/**
 * TAML (Tab Accessible Markup Language) Parser and Serializer
 * Version 0.1.0
 */

const TAB = '\t';
const NULL_VALUE = '~';
const EMPTY_STRING = '""';
const RAW_TEXT = '...';

const TRUTHY_VALUES = new Set(['true', 'yes', 'on'])
const FALSY_VALUES = new Set(['false', 'no', 'off'])

const ISO_DATE_RE = /^\d{4}-\d{2}-\d{2}(?:T\d{2}:\d{2}(?::\d{2}(?:\.\d+)?)?(?:Z|[+-]\d{2}:\d{2})?)?$/

export class TAMLError extends Error {
  constructor(message, line) {
    super(line !== undefined ? `Line ${line}: ${message}` : message);
    this.name = 'TAMLError';
    this.line = line;
  }
}

/**
 * Parse a TAML string into a JavaScript object
 * @param {string} text - TAML formatted text
 * @param {Object} options - Parsing options
 * @param {boolean} options.strict - Enable strict parsing (default: false)
 * @param {boolean} options.typeConversion - Convert string values to native types (default: true)
 * @returns {Object} Parsed JavaScript object
 */
export function parse(text, options = {}){
  const { strict = false, typeConversion = true } = options;
  
  const lines = text.split('\n');
  const root = {};
  const stack = [{ level: -1, node: root }];
  
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i];
    const lineNum = i + 1;
    
    if (line.trim() === '') continue;
    if (line.trimStart().startsWith('#')) continue;
    
    if (line.match(/^[ \t]+/) && line.match(/^ /)) {
      if (strict) {
        throw new TAMLError('Indentation must use tabs, not spaces', lineNum);
      }
      continue;
    }
    
    const indentMatch = line.match(/^(\t*)/);
    const level = indentMatch ? indentMatch[1].length : 0;
    const content = line.substring(level);
    
    if (content.trim() === '') continue;
    
    const tabIndex = content.indexOf(TAB);
    const hasValue = tabIndex !== -1;
    const key = hasValue ? content.substring(0, tabIndex) : content.trim();
    const rawValue = hasValue ? content.substring(tabIndex + 1).replace(/^\t+/, '').trimEnd() : null;
    
    if (!key) {
      if (strict) throw new TAMLError('Line has no key', lineNum);
      continue;
    }
    
    if (rawValue && rawValue !== RAW_TEXT && rawValue.includes(TAB)) {
      if (strict) throw new TAMLError('Value contains invalid tab character', lineNum);
      continue;
    }
    
    // Handle raw text blocks
    if (rawValue === RAW_TEXT) {
      const baseIndent = level + 1;
      const rawLines = [];
      let j = i + 1;
      while (j < lines.length) {
        const rawLine = lines[j];
        // Empty lines within raw text are preserved
        if (rawLine.trim() === '') {
          rawLines.push('');
          j++;
          continue;
        }
        const rawIndent = (rawLine.match(/^(\t*)/) || ['', ''])[1].length;
        if (rawIndent < baseIndent) break;
        // Strip structural indentation, preserve additional tabs
        rawLines.push(rawLine.substring(baseIndent));
        j++;
      }
      // Trim trailing empty lines
      while (rawLines.length > 0 && rawLines[rawLines.length - 1] === '') {
        rawLines.pop();
      }
      const value = rawLines.join('\n');
      i = j - 1;
      
      while (stack.length > 1 && stack[stack.length - 1].level >= level) {
        stack.pop();
      }
      const parent = stack[stack.length - 1];
      if (Array.isArray(parent.node)) {
        parent.node.push(value);
      } else {
        parent.node[key] = value;
      }
      continue;
    }
    
    let value = rawValue;
    if (rawValue === NULL_VALUE) {
      value = null;
    } else if (rawValue === EMPTY_STRING) {
      value = '';
    } else if (rawValue !== null && rawValue !== '' && typeConversion) {
      value = convertType(rawValue);
    }
    
    while (stack.length > 1 && stack[stack.length - 1].level >= level) {
      stack.pop();
    }
    
    const parent = stack[stack.length - 1];
    
    if (level > parent.level + 1) {
      if (strict) {
        throw new TAMLError(`Invalid indentation level (expected ${parent.level + 1} tabs, found ${level})`, lineNum);
      }
      continue;
    }
    
    if (Array.isArray(parent.node)) {
      // Parent is an array
      if (hasValue) {
        if (strict) {
          throw new TAMLError('List items cannot be key-value pairs', lineNum);
        }
        continue;
      }
      // Collection of objects: bare key with children → push new object
      if (parent.collectKey) {
        const obj = {};
        parent.node.push(obj);
        stack.push({ level, node: obj });
      } else {
        parent.node.push(key);
      }
    } else {
      // Parent is an object
      if (hasValue) {
        // Leaf value
        parent.node[key] = value;
      } else {
        // Check for duplicate bare key at same level (key already exists)
        if (key in parent.node) {
          const existing = parent.node[key];
          if (typeof existing === 'object' && existing !== null && !Array.isArray(existing)) {
            // Convert single object to array
            parent.node[key] = [existing];
          }
          if (Array.isArray(parent.node[key])) {
            const newObj = {};
            parent.node[key].push(newObj);
            stack.push({ level, node: newObj });
            continue;
          }
        }
        
        // Parent node - look ahead to determine if it's an array or object
        let isArray = false;
        let hasListItems = false;
        let hasKeyValuePairs = false;
        let hasDuplicateBareKeys = false;
        const bareKeyNames = new Set();
        
        for (let j = i + 1; j < lines.length; j++) {
          const nextLine = lines[j];
          if (nextLine.trim() === '' || nextLine.trimStart().startsWith('#')) {
            continue;
          }
          const nextIndent = (nextLine.match(/^(\t*)/) || ['', ''])[1].length;
          
          if (nextIndent < level + 1) {
            break;
          }
          
          if (nextIndent === level + 1) {
            const nextContent = nextLine.substring(nextIndent);
            const nextTabIdx = nextContent.indexOf(TAB);
            
            if (nextTabIdx > 0) {
              hasKeyValuePairs = true;
            } else if (nextTabIdx === -1) {
              // No tab - could be list item or parent node
              const bareKey = nextContent.trim();
              let hasChildren = false;
              for (let k = j + 1; k < lines.length; k++) {
                const checkLine = lines[k];
                if (checkLine.trim() === '' || checkLine.trimStart().startsWith('#')) {
                  continue;
                }
                const checkIndent = (checkLine.match(/^(\t*)/) || ['', ''])[1].length;
                if (checkIndent > nextIndent) {
                  hasChildren = true;
                  break;
                } else if (checkIndent <= nextIndent) {
                  break;
                }
              }
              
              if (hasChildren) {
                // Check for duplicate bare keys
                if (bareKeyNames.has(bareKey)) {
                  hasDuplicateBareKeys = true;
                }
                bareKeyNames.add(bareKey);
              } else {
                hasListItems = true;
              }
            }
          }
        }
        
        if (hasDuplicateBareKeys) {
          isArray = true;
        } else {
          isArray = hasListItems && !hasKeyValuePairs;
        }
        
        // Handle duplicate bare keys at current level
        if (hasDuplicateBareKeys) {
          parent.node[key] = [];
          stack.push({ level, node: parent.node[key], collectKey: [...bareKeyNames][0] });
        } else {
          parent.node[key] = isArray ? [] : {};
          stack.push({ level, node: parent.node[key] });
        }
      }
    }
  }
  
  return root;
}

/**
 * Convert string value to native type
 */
function convertType(value) {
  const lower = value.toLowerCase()

  // Boolean detection (case-insensitive)
  if (TRUTHY_VALUES.has(lower)) return true
  if (FALSY_VALUES.has(lower)) return false

  // ISO 8601 date detection (before number detection)
  if (ISO_DATE_RE.test(value)) {
    const d = new Date(value)
    if (!isNaN(d.getTime())) return d
  }

  // Integer detection
  if (/^-?\d+$/.test(value)) {
    return parseInt(value, 10)
  }

  // Float detection
  if (/^-?\d+\.\d+$/.test(value)) {
    return parseFloat(value)
  }

  return value
}

/**
 * Serialize a JavaScript object to TAML format
 * @param {*} obj - JavaScript object to serialize
 * @param {Object} options - Serialization options
 * @param {number} options.indentLevel - Starting indentation level (default: 0)
 * @param {boolean} options.typeConversion - Convert native types to strings (default: true)
 * @returns {string} TAML formatted string
 */
export function stringify(obj, options = {}) {
  const { indentLevel = 0, typeConversion = true } = options;
  const lines = [];
  
  serializeValue(obj, lines, indentLevel, typeConversion);
  
  return lines.join('\n');
}

function serializeValue(value, lines, level, typeConversion) {
  if (value === null) {
    return NULL_VALUE;
  }
  
  if (value === '') {
    return EMPTY_STRING;
  }
  
  if (value instanceof Date) {
    return value.toISOString();
  }
  
  if (typeof value === 'string') {
    // Check if string needs raw text block (contains tabs or newlines)
    if (value.includes('\t') || value.includes('\n')) {
      return RAW_TEXT;
    }
    return value;
  }
  
  if (typeof value === 'number' || typeof value === 'boolean') {
    return String(value);
  }
  
  if (Array.isArray(value)) {
    // Check if this is a collection of objects (all items are plain objects)
    const isCollectionOfObjects = value.length > 0 && value.every(
      item => typeof item === 'object' && item !== null && !Array.isArray(item) && !(item instanceof Date)
    );
    if (isCollectionOfObjects) {
      // Handled by the caller (serializeObject) which knows the key name
      return null;
    }
    for (const item of value) {
      const indent = TAB.repeat(level);
      if (typeof item === 'object' && item !== null && !(item instanceof Date)) {
        serializeObject(item, lines, level, typeConversion);
      } else {
        const serialized = serializeValue(item, [], level, typeConversion);
        lines.push(indent + serialized);
      }
    }
    return null;
  }
  
  if (typeof value === 'object') {
    serializeObject(value, lines, level, typeConversion);
    return null;
  }
  
  return String(value);
}

function serializeRawText(key, value, lines, level) {
  const indent = TAB.repeat(level);
  const contentIndent = TAB.repeat(level + 1);
  lines.push(indent + key + TAB + RAW_TEXT);
  const rawLines = value.split('\n');
  for (const rawLine of rawLines) {
    lines.push(contentIndent + rawLine);
  }
}

function serializeObject(obj, lines, level, typeConversion) {
  const indent = TAB.repeat(level);
  
  for (const [key, value] of Object.entries(obj)) {
    if (value === null) {
      lines.push(indent + key + TAB + NULL_VALUE);
    } else if (value === '') {
      lines.push(indent + key + TAB + EMPTY_STRING);
    } else if (value instanceof Date) {
      lines.push(indent + key + TAB + value.toISOString());
    } else if (typeof value === 'string' && (value.includes('\t') || value.includes('\n'))) {
      serializeRawText(key, value, lines, level);
    } else if (typeof value === 'object') {
      if (Array.isArray(value)) {
        // Check for collection of objects → duplicate bare keys
        const isCollectionOfObjects = value.length > 0 && value.every(
          item => typeof item === 'object' && item !== null && !Array.isArray(item) && !(item instanceof Date)
        );
        if (isCollectionOfObjects) {
          lines.push(indent + key);
          const childIndent = TAB.repeat(level + 1);
          for (const item of value) {
            lines.push(childIndent + key);
            serializeObject(item, lines, level + 2, typeConversion);
          }
        } else {
          lines.push(indent + key);
          serializeValue(value, lines, level + 1, typeConversion);
        }
      } else {
        lines.push(indent + key);
        serializeObject(value, lines, level + 1, typeConversion);
      }
    } else {
      const serialized = serializeValue(value, [], level, typeConversion);
      lines.push(indent + key + TAB + serialized);
    }
  }
}

export default {
  parse,
  stringify,
  TAMLError
};
