"""Tests for TAML parser"""

import unittest
from taml import parse, TAMLError


class TestParser(unittest.TestCase):
    
    def test_simple_key_value(self):
        """Test parsing simple key-value pairs"""
        text = "name\tvalue\nkey\tdata"
        result = parse(text)
        self.assertEqual(result, {'name': 'value', 'key': 'data'})
    
    def test_nested_structure(self):
        """Test parsing nested objects"""
        text = "parent\n\tchild\tvalue\n\tanother\tdata"
        result = parse(text)
        self.assertEqual(result, {
            'parent': {
                'child': 'value',
                'another': 'data'
            }
        })
    
    def test_list_items(self):
        """Test parsing lists"""
        text = "items\n\tfirst\n\tsecond\n\tthird"
        result = parse(text)
        self.assertEqual(result, {
            'items': ['first', 'second', 'third']
        })
    
    def test_type_conversion(self):
        """Test automatic type conversion"""
        text = "number\t42\nfloat\t3.14\nbool_true\ttrue\nbool_false\tfalse"
        result = parse(text)
        self.assertEqual(result['number'], 42)
        self.assertEqual(result['float'], 3.14)
        self.assertEqual(result['bool_true'], True)
        self.assertEqual(result['bool_false'], False)
    
    def test_no_type_conversion(self):
        """Test disabled type conversion"""
        text = "number\t42\nbool\ttrue"
        result = parse(text, type_conversion=False)
        self.assertEqual(result['number'], '42')
        self.assertEqual(result['bool'], 'true')
    
    def test_null_value(self):
        """Test null value handling"""
        text = "key\t~"
        result = parse(text)
        self.assertIsNone(result['key'])
    
    def test_empty_string(self):
        """Test empty string handling"""
        text = 'key\t""'
        result = parse(text)
        self.assertEqual(result['key'], '')
    
    def test_comments(self):
        """Test comment handling"""
        text = "# Comment\nkey\tvalue\n# Another comment\nname\tdata"
        result = parse(text)
        self.assertEqual(result, {'key': 'value', 'name': 'data'})
    
    def test_blank_lines(self):
        """Test blank line handling"""
        text = "key\tvalue\n\n\nname\tdata"
        result = parse(text)
        self.assertEqual(result, {'key': 'value', 'name': 'data'})
    
    def test_multiple_tabs_separator(self):
        """Test multiple tabs as separator"""
        text = "key\t\t\tvalue"
        result = parse(text)
        self.assertEqual(result, {'key': 'value'})
    
    def test_deeply_nested(self):
        """Test deeply nested structures"""
        text = "level1\n\tlevel2\n\t\tlevel3\n\t\t\tkey\tvalue"
        result = parse(text)
        self.assertEqual(result, {
            'level1': {
                'level2': {
                    'level3': {
                        'key': 'value'
                    }
                }
            }
        })
    
    def test_mixed_content(self):
        """Test mixed lists and objects"""
        text = """config
\tdatabase
\t\thost\tlocalhost
\t\tport\t5432
\tfeatures
\t\tauthentication
\t\tlogging"""
        result = parse(text)
        self.assertEqual(result, {
            'config': {
                'database': {
                    'host': 'localhost',
                    'port': 5432
                },
                'features': ['authentication', 'logging']
            }
        })
    
    def test_strict_space_indentation(self):
        """Test strict mode rejects space indentation"""
        text = "parent\n    child\tvalue"
        with self.assertRaises(TAMLError) as cm:
            parse(text, strict=True)
        self.assertIn('Indentation must use tabs', str(cm.exception))
    
    def test_strict_tab_in_value(self):
        """Test strict mode rejects tabs in values"""
        text = "key\tvalue\twith\ttab"
        with self.assertRaises(TAMLError) as cm:
            parse(text, strict=True)
        self.assertIn('tab character', str(cm.exception))
    
    def test_lenient_space_indentation(self):
        """Test lenient mode skips lines with space indentation"""
        text = "key\tvalue\n    bad\tline\nname\tdata"
        result = parse(text, strict=False)
        self.assertEqual(result, {'key': 'value', 'name': 'data'})
    
    def test_complex_example(self):
        """Test complex TAML document"""
        text = """application\tMyApp
version\t1.0.0
author\tDeveloper Name
license\t~

server
\thost\t0.0.0.0
\tport\t8080
\tssl\ttrue

database
\ttype\tpostgresql
\tconnection
\t\thost\tdb.example.com
\t\tport\t5432
\t\tdatabase\tmyapp_db
\t\tpassword\t~

features
\tuser-authentication
\tapi-gateway
\trate-limiting
\tlogging"""
        result = parse(text)
        
        self.assertEqual(result['application'], 'MyApp')
        self.assertEqual(result['version'], '1.0.0')
        self.assertIsNone(result['license'])
        self.assertEqual(result['server']['port'], 8080)
        self.assertEqual(result['server']['ssl'], True)
        self.assertEqual(result['database']['connection']['host'], 'db.example.com')
        self.assertIsNone(result['database']['connection']['password'])
        self.assertEqual(len(result['features']), 4)
        self.assertIn('logging', result['features'])


if __name__ == '__main__':
    unittest.main()
