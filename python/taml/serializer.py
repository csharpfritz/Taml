"""TAML Serializer - Convert Python objects to TAML format"""

from typing import Any, List

TAB = '\t'
NULL_VALUE = '~'
EMPTY_STRING = '""'


def stringify(obj: Any, indent_level: int = 0, type_conversion: bool = True) -> str:
    """
    Serialize a Python object to TAML format
    
    Args:
        obj: Python object to serialize
        indent_level: Starting indentation level (default: 0)
        type_conversion: Convert native types to strings (default: True)
    
    Returns:
        TAML formatted string
    """
    lines: List[str] = []
    _serialize_value(obj, lines, indent_level, type_conversion)
    return '\n'.join(lines)


def _serialize_value(value: Any, lines: List[str], level: int, type_conversion: bool) -> str:
    """Serialize a value and return its string representation"""
    if value is None:
        return NULL_VALUE
    
    if value == '':
        return EMPTY_STRING
    
    if isinstance(value, str):
        return value
    
    if isinstance(value, bool):
        return 'true' if value else 'false'
    
    if isinstance(value, (int, float)):
        return str(value)
    
    if isinstance(value, (list, tuple)):
        for item in value:
            indent = TAB * level
            if isinstance(item, dict):
                _serialize_object(item, lines, level, type_conversion)
            elif isinstance(item, (list, tuple)):
                _serialize_value(item, lines, level, type_conversion)
            else:
                serialized = _serialize_value(item, [], level, type_conversion)
                lines.append(indent + serialized)
        return ''
    
    if isinstance(value, dict):
        _serialize_object(value, lines, level, type_conversion)
        return ''
    
    return str(value)


def _serialize_object(obj: dict, lines: List[str], level: int, type_conversion: bool) -> None:
    """Serialize a dictionary object"""
    indent = TAB * level
    
    for key, value in obj.items():
        if value is None:
            lines.append(indent + key + TAB + NULL_VALUE)
        elif value == '':
            lines.append(indent + key + TAB + EMPTY_STRING)
        elif isinstance(value, dict):
            lines.append(indent + key)
            _serialize_object(value, lines, level + 1, type_conversion)
        elif isinstance(value, (list, tuple)):
            lines.append(indent + key)
            _serialize_value(value, lines, level + 1, type_conversion)
        else:
            serialized = _serialize_value(value, [], level, type_conversion)
            lines.append(indent + key + TAB + serialized)
