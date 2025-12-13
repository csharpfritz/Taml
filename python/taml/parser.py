"""TAML Parser - Parse TAML formatted text into Python objects"""

from typing import Any, Dict, List, Optional, Union

TAB = '\t'
NULL_VALUE = '~'
EMPTY_STRING = '""'


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
    
    for i, line in enumerate(lines):
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
                # Parent node - determine if it's a list or dict
                is_array = _is_array_parent(lines, i, level)
                parent_node[key] = [] if is_array else {}
                stack.append({'level': level, 'node': parent_node[key]})
    
    return root


def _convert_type(value: str) -> Union[str, int, float, bool]:
    """Convert string value to native Python type"""
    if value == 'true':
        return True
    if value == 'false':
        return False
    
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
