"""Tests for TAML serializer"""

import unittest
from taml import stringify


class TestSerializer(unittest.TestCase):
    
    def test_simple_key_value(self):
        """Test serializing simple key-value pairs"""
        data = {'name': 'value', 'key': 'data'}
        result = stringify(data)
        self.assertIn('name\tvalue', result)
        self.assertIn('key\tdata', result)
    
    def test_nested_structure(self):
        """Test serializing nested objects"""
        data = {
            'parent': {
                'child': 'value',
                'another': 'data'
            }
        }
        result = stringify(data)
        lines = result.split('\n')
        self.assertIn('parent', lines[0])
        self.assertIn('\tchild\tvalue', result)
        self.assertIn('\tanother\tdata', result)
    
    def test_list_items(self):
        """Test serializing lists"""
        data = {
            'items': ['first', 'second', 'third']
        }
        result = stringify(data)
        self.assertIn('items', result)
        self.assertIn('\tfirst', result)
        self.assertIn('\tsecond', result)
        self.assertIn('\tthird', result)
    
    def test_type_conversion(self):
        """Test type conversion during serialization"""
        data = {
            'number': 42,
            'float': 3.14,
            'bool_true': True,
            'bool_false': False
        }
        result = stringify(data)
        self.assertIn('number\t42', result)
        self.assertIn('float\t3.14', result)
        self.assertIn('bool_true\ttrue', result)
        self.assertIn('bool_false\tfalse', result)
    
    def test_null_value(self):
        """Test null value serialization"""
        data = {'key': None}
        result = stringify(data)
        self.assertIn('key\t~', result)
    
    def test_empty_string(self):
        """Test empty string serialization"""
        data = {'key': ''}
        result = stringify(data)
        self.assertIn('key\t""', result)
    
    def test_deeply_nested(self):
        """Test deeply nested structure serialization"""
        data = {
            'level1': {
                'level2': {
                    'level3': {
                        'key': 'value'
                    }
                }
            }
        }
        result = stringify(data)
        self.assertIn('level1', result)
        self.assertIn('\tlevel2', result)
        self.assertIn('\t\tlevel3', result)
        self.assertIn('\t\t\tkey\tvalue', result)
    
    def test_mixed_content(self):
        """Test mixed lists and objects"""
        data = {
            'config': {
                'database': {
                    'host': 'localhost',
                    'port': 5432
                },
                'features': ['authentication', 'logging']
            }
        }
        result = stringify(data)
        self.assertIn('config', result)
        self.assertIn('\tdatabase', result)
        self.assertIn('\t\thost\tlocalhost', result)
        self.assertIn('\t\tport\t5432', result)
        self.assertIn('\tfeatures', result)
        self.assertIn('\t\tauthentication', result)
        self.assertIn('\t\tlogging', result)
    
    def test_complex_example(self):
        """Test complex object serialization"""
        data = {
            'application': 'MyApp',
            'version': '1.0.0',
            'license': None,
            'server': {
                'host': '0.0.0.0',
                'port': 8080,
                'ssl': True
            },
            'database': {
                'type': 'postgresql',
                'connection': {
                    'host': 'db.example.com',
                    'port': 5432,
                    'password': None
                }
            },
            'features': [
                'authentication',
                'logging',
                'caching'
            ]
        }
        result = stringify(data)
        
        self.assertIn('application\tMyApp', result)
        self.assertIn('version\t1.0.0', result)
        self.assertIn('license\t~', result)
        self.assertIn('server', result)
        self.assertIn('\thost\t0.0.0.0', result)
        self.assertIn('\tport\t8080', result)
        self.assertIn('\tssl\ttrue', result)
        self.assertIn('database', result)
        self.assertIn('\ttype\tpostgresql', result)
        self.assertIn('\tconnection', result)
        self.assertIn('\t\thost\tdb.example.com', result)
        self.assertIn('\t\tpassword\t~', result)
        self.assertIn('features', result)
        self.assertIn('\tauthentication', result)


if __name__ == '__main__':
    unittest.main()
