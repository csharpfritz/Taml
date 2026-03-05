"""TAML Parser - Parse TAML formatted text into Python objects"""

import re
from datetime import date, datetime, timezone
from typing import Any, Dict, List, Optional, Tuple, Union

TAB = '\t'
NULL_VALUE = '~'
EMPTY_STRING = '""'
RAW_TEXT_MARKER = '...'

# Boolean truthy/falsy values (lowercase, case-insensitive detection)
_TRUTHY_VALUES = frozenset({'true', 'yes', 'on'})
_FALSY_VALUES = frozenset({'false', 'no', 'off'})

# ISO 8601 date/time patterns
_YEAR_MONTH_RE = re.compile(r'^(\d{4})-(\d{2})$')
_DATE_ONLY_RE = re.compile(r'^\d{4}-\d{2}-\d{2}$')
_DATETIME_RE = re.compile(
    r'^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}'
    r'(?:\.\d+)?'
    r'(?:Z|[+-]\d{2}:\d{2})?$'
)


class TAMLError(Exception):
    """Exception raised for TAML parsing errors"""
    
    def __init__(self, message: str, line: Optional[int] = None):
        self.line = line
        if line is not None:
            super().__init__(f"Line {line}: {message}")
        else:
            super().__init__(message)


def parse(text: str, strict: bool = False, type_conversion: bool = True) -> Dict[str, Any]:
    """
    Parse a TAML string into a Python dictionary
    
    Args:
        text: TAML formatted text
        strict: Enable strict parsing (default: False)
        type_conversion: Convert string values to native types (default: True)
    
    Returns:
        Parsed Python dictionary
    
    Raises:
        TAMLError: If parsing fails in strict mode
    """
    lines = text.split('\n')
    root: Dict[str, Any] = {}
    stack: List[Dict[str, Any]] = [{'level': -1, 'node': root}]
    skip_to: int = -1
    
    for i, line in enumerate(lines):
        if i < skip_to:
            continue
        
        line_num = i + 1
        
        # Skip empty lines
        if not line.strip():
            continue
        
        # Skip comments
        if line.lstrip().startswith('#'):
            continue
        
        # Check for space indentation
        if line and line[0] == ' ':
            if strict:
                raise TAMLError('Indentation must use tabs, not spaces', line_num)
            continue
        
        # Count leading tabs
        level = 0
        for char in line:
            if char == '\t':
                level += 1
            elif char == ' ':
                if strict:
                    raise TAMLError('Mixed spaces and tabs in indentation', line_num)
                break
            else:
                break
        
        content = line[level:]
        
        if not content.strip():
            continue
        
        # Check for key-value separator
        tab_index = content.find(TAB)
        has_value = tab_index != -1
        
        if has_value:
            key = content[:tab_index]
            # Skip all separator tabs
            value_start = tab_index
            while value_start < len(content) and content[value_start] == TAB:
                value_start += 1
            raw_value = content[value_start:].rstrip() if value_start < len(content) else ''
        else:
            key = content.rstrip()
            raw_value = None
        
        if not key:
            if strict:
                raise TAMLError('Line has no key', line_num)
            continue
        
        # Handle raw text blocks
        if raw_value == RAW_TEXT_MARKER:
            while len(stack) > 1 and stack[-1]['level'] >= level:
                stack.pop()
            
            parent = stack[-1]
            if level > parent['level'] + 1:
                if strict:
                    raise TAMLError(
                        f"Invalid indentation level (expected {parent['level'] + 1} tabs, found {level})",
                        line_num
                    )
                continue
            
            parent_node = parent['node']
            raw_content, skip_to = _collect_raw_text(lines, i, level)
            if isinstance(parent_node, dict):
                parent_node[key] = raw_content
            continue
        
        # Check for tabs in value
        if raw_value and TAB in raw_value:
            if strict:
                raise TAMLError('Value contains invalid tab character', line_num)
            continue
        
        # Convert value
        value: Any
        if raw_value == NULL_VALUE:
            value = None
        elif raw_value == EMPTY_STRING:
            value = ''
        elif raw_value is not None and raw_value != '' and type_conversion:
            value = _convert_type(raw_value)
        else:
            value = raw_value
        
        # Pop stack to correct level
        while len(stack) > 1 and stack[-1]['level'] >= level:
            stack.pop()
        
        parent = stack[-1]
        
        # Check indentation level
        if level > parent['level'] + 1:
            if strict:
                raise TAMLError(
                    f"Invalid indentation level (expected {parent['level'] + 1} tabs, found {level})",
                    line_num
                )
            continue
        
        parent_node = parent['node']
        
        if isinstance(parent_node, list):
            # Parent is a list, this is a list item
            if has_value:
                if strict:
                    raise TAMLError('List items cannot be key-value pairs', line_num)
                continue
            parent_node.append(key)
        else:
            # Parent is a dict
            if has_value:
                # Leaf value
                parent_node[key] = value
            else:
                # Duplicate bare key → collection of objects
                if key in parent_node and isinstance(parent_node[key], dict):
                    existing = parent_node[key]
                    new_dict: Dict[str, Any] = {}
                    parent_node[key] = [existing, new_dict]
                    stack.append({'level': level, 'node': new_dict})
                elif (key in parent_node
                      and isinstance(parent_node[key], list)
                      and parent_node[key]
                      and isinstance(parent_node[key][0], dict)):
                    new_dict = {}
                    parent_node[key].append(new_dict)
                    stack.append({'level': level, 'node': new_dict})
                else:
                    # New key - determine if it's a list or dict
                    is_array = _is_array_parent(lines, i, level)
                    parent_node[key] = [] if is_array else {}
                    stack.append({'level': level, 'node': parent_node[key]})
    
    return root


def _convert_type(value: str) -> Union[str, int, float, bool, date, datetime]:
    """
    Convert string value to native Python type
    
    Detection order: booleans → dates → numbers → strings
    
    Args:
        value: String value to convert
    
    Returns:
        Converted value as the appropriate Python type
    """
    # Booleans (case-insensitive)
    lower = value.lower()
    if lower in _TRUTHY_VALUES:
        return True
    if lower in _FALSY_VALUES:
        return False
    
    # ISO 8601 dates/times
    date_result = _try_parse_date(value)
    if date_result is not None:
        return date_result
    
    # Try integer
    if value.lstrip('-').isdigit():
        return int(value)
    
    # Try float
    try:
        if '.' in value:
            return float(value)
    except ValueError:
        pass
    
    return value


def _try_parse_date(value: str) -> Optional[Union[date, datetime]]:
    """
    Try to parse an ISO 8601 date or datetime string
    
    Args:
        value: String value to attempt to parse
    
    Returns:
        A date or datetime object if the value matches ISO 8601, None otherwise
    """
    # Year-month only: YYYY-MM → date with day=1
    m = _YEAR_MONTH_RE.match(value)
    if m:
        try:
            return date(int(m.group(1)), int(m.group(2)), 1)
        except ValueError:
            return None

    # Date only: YYYY-MM-DD
    if _DATE_ONLY_RE.match(value):
        try:
            return date.fromisoformat(value)
        except ValueError:
            return None
    
    # Datetime: YYYY-MM-DDTHH:MM:SS with optional timezone
    if _DATETIME_RE.match(value):
        try:
            # Handle Z suffix for broader Python compatibility
            parsed_value = value.replace('Z', '+00:00') if value.endswith('Z') else value
            return datetime.fromisoformat(parsed_value)
        except ValueError:
            return None
    
    return None


def _collect_raw_text(lines: List[str], current_index: int, parent_level: int) -> Tuple[str, int]:
    """
    Collect raw text content from lines following a ... indicator
    
    Args:
        lines: All document lines
        current_index: Index of the line containing the ... value
        parent_level: Indentation level of the parent key
    
    Returns:
        Tuple of (raw text content string, next line index to process)
    """
    base_indent = parent_level + 1
    raw_lines: List[str] = []
    j = current_index + 1
    
    while j < len(lines):
        raw_line = lines[j]
        
        # Blank/empty lines are preserved as empty content lines
        if not raw_line.strip():
            raw_lines.append('')
            j += 1
            continue
        
        # Count indentation
        indent = 0
        for ch in raw_line:
            if ch == '\t':
                indent += 1
            else:
                break
        
        # End raw text if indentation drops below required level
        if indent < base_indent:
            break
        
        # Strip structural indentation, preserve remaining content
        raw_lines.append(raw_line[base_indent:])
        j += 1
    
    # Remove trailing empty lines
    while raw_lines and raw_lines[-1] == '':
        raw_lines.pop()
    
    return '\n'.join(raw_lines), j


def _is_array_parent(lines: List[str], current_index: int, level: int) -> bool:
    """Determine if a parent node should be an array or dict"""
    has_list_items = False
    has_key_value_pairs = False
    
    for j in range(current_index + 1, len(lines)):
        next_line = lines[j]
        
        if not next_line.strip() or next_line.lstrip().startswith('#'):
            continue
        
        # Count indentation
        next_indent = 0
        for char in next_line:
            if char == '\t':
                next_indent += 1
            else:
                break
        
        # Stop if back to same or lower level
        if next_indent < level + 1:
            break
        
        # Only check immediate children
        if next_indent == level + 1:
            next_content = next_line[next_indent:]
            next_tab_idx = next_content.find(TAB)
            
            if next_tab_idx > 0:
                # Has key-value with tab separator
                has_key_value_pairs = True
            elif next_tab_idx == -1:
                # No tab - could be list item or parent node
                # Check if it has children
                has_children = _has_children(lines, j, next_indent)
                if not has_children:
                    has_list_items = True
    
    return has_list_items and not has_key_value_pairs


def _has_children(lines: List[str], current_index: int, level: int) -> bool:
    """Check if a line has children"""
    for k in range(current_index + 1, len(lines)):
        check_line = lines[k]
        
        if not check_line.strip() or check_line.lstrip().startswith('#'):
            continue
        
        check_indent = 0
        for char in check_line:
            if char == '\t':
                check_indent += 1
            else:
                break
        
        if check_indent > level:
            return True
        elif check_indent <= level:
            break
    
    return False
