"""Tests for TAML round-trip (parse -> stringify -> parse)"""

import unittest
from taml import parse, stringify


class TestRoundTrip(unittest.TestCase):
    
    def test_simple_roundtrip(self):
        """Test round-trip of simple key-value pairs"""
        original = {'name': 'value', 'key': 'data'}
        taml = stringify(original)
        result = parse(taml)
        self.assertEqual(result, original)
    
    def test_nested_roundtrip(self):
        """Test round-trip of nested objects"""
        original = {
            'parent': {
                'child': 'value',
                'another': 'data'
            }
        }
        taml = stringify(original)
        result = parse(taml)
        self.assertEqual(result, original)
    
    def test_list_roundtrip(self):
        """Test round-trip of lists"""
        original = {
            'items': ['first', 'second', 'third']
        }
        taml = stringify(original)
        result = parse(taml)
        self.assertEqual(result, original)
    
    def test_types_roundtrip(self):
        """Test round-trip of different types"""
        original = {
            'string': 'text',
            'number': 42,
            'float': 3.14,
            'bool_true': True,
            'bool_false': False,
            'null': None,
            'empty': ''
        }
        taml = stringify(original)
        result = parse(taml)
        self.assertEqual(result, original)
    
    def test_complex_roundtrip(self):
        """Test round-trip of complex structure"""
        original = {
            'application': 'MyApp',
            'version': '1.0.0',
            'server': {
                'host': 'localhost',
                'port': 8080,
                'ssl': True
            },
            'database': {
                'connection': {
                    'host': 'db.example.com',
                    'password': None
                }
            },
            'features': ['authentication', 'logging', 'caching']
        }
        taml = stringify(original)
        result = parse(taml)
        self.assertEqual(result, original)
    
    def test_deeply_nested_roundtrip(self):
        """Test round-trip of deeply nested structure"""
        original = {
            'level1': {
                'level2': {
                    'level3': {
                        'level4': {
                            'key': 'value'
                        }
                    }
                }
            }
        }
        taml = stringify(original)
        result = parse(taml)
        self.assertEqual(result, original)


if __name__ == '__main__':
    unittest.main()
