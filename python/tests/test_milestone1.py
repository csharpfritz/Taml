"""Tests for Milestone 1 features: extended booleans, raw text, dates, duplicate keys"""

import unittest
from datetime import date, datetime, timezone, timedelta
from taml import parse, stringify


class TestExtendedBooleans(unittest.TestCase):
    """Feature 1: Extended boolean recognition (case-insensitive)"""
    
    def test_true_lowercase(self):
        """Test 'true' is parsed as True"""
        result = parse("key\ttrue")
        self.assertIs(result['key'], True)
    
    def test_true_uppercase(self):
        """Test 'TRUE' is parsed as True"""
        result = parse("key\tTRUE")
        self.assertIs(result['key'], True)
    
    def test_true_mixed_case(self):
        """Test 'True' is parsed as True"""
        result = parse("key\tTrue")
        self.assertIs(result['key'], True)
    
    def test_yes_lowercase(self):
        """Test 'yes' is parsed as True"""
        result = parse("key\tyes")
        self.assertIs(result['key'], True)
    
    def test_yes_uppercase(self):
        """Test 'YES' is parsed as True"""
        result = parse("key\tYES")
        self.assertIs(result['key'], True)
    
    def test_yes_mixed_case(self):
        """Test 'Yes' is parsed as True"""
        result = parse("key\tYes")
        self.assertIs(result['key'], True)
    
    def test_on_lowercase(self):
        """Test 'on' is parsed as True"""
        result = parse("key\ton")
        self.assertIs(result['key'], True)
    
    def test_on_uppercase(self):
        """Test 'ON' is parsed as True"""
        result = parse("key\tON")
        self.assertIs(result['key'], True)
    
    def test_on_mixed_case(self):
        """Test 'On' is parsed as True"""
        result = parse("key\tOn")
        self.assertIs(result['key'], True)
    
    def test_false_lowercase(self):
        """Test 'false' is parsed as False"""
        result = parse("key\tfalse")
        self.assertIs(result['key'], False)
    
    def test_false_uppercase(self):
        """Test 'FALSE' is parsed as False"""
        result = parse("key\tFALSE")
        self.assertIs(result['key'], False)
    
    def test_false_mixed_case(self):
        """Test 'False' is parsed as False"""
        result = parse("key\tFalse")
        self.assertIs(result['key'], False)
    
    def test_no_lowercase(self):
        """Test 'no' is parsed as False"""
        result = parse("key\tno")
        self.assertIs(result['key'], False)
    
    def test_no_uppercase(self):
        """Test 'NO' is parsed as False"""
        result = parse("key\tNO")
        self.assertIs(result['key'], False)
    
    def test_no_mixed_case(self):
        """Test 'No' is parsed as False"""
        result = parse("key\tNo")
        self.assertIs(result['key'], False)
    
    def test_off_lowercase(self):
        """Test 'off' is parsed as False"""
        result = parse("key\toff")
        self.assertIs(result['key'], False)
    
    def test_off_uppercase(self):
        """Test 'OFF' is parsed as False"""
        result = parse("key\tOFF")
        self.assertIs(result['key'], False)
    
    def test_off_mixed_case(self):
        """Test 'Off' is parsed as False"""
        result = parse("key\tOff")
        self.assertIs(result['key'], False)
    
    def test_one_is_not_boolean(self):
        """Test '1' stays as integer, NOT boolean"""
        result = parse("key\t1")
        self.assertEqual(result['key'], 1)
        self.assertIsInstance(result['key'], int)
        self.assertNotIsInstance(result['key'], bool)
    
    def test_zero_is_not_boolean(self):
        """Test '0' stays as integer, NOT boolean"""
        result = parse("key\t0")
        self.assertEqual(result['key'], 0)
        self.assertIsInstance(result['key'], int)
        self.assertNotIsInstance(result['key'], bool)
    
    def test_serialize_true_canonical(self):
        """Test True serializes to lowercase 'true'"""
        result = stringify({'key': True})
        self.assertIn('key\ttrue', result)
    
    def test_serialize_false_canonical(self):
        """Test False serializes to lowercase 'false'"""
        result = stringify({'key': False})
        self.assertIn('key\tfalse', result)
    
    def test_bool_roundtrip_yes(self):
        """Test 'yes' parses to True and serializes back as 'true'"""
        data = parse("enabled\tyes")
        self.assertIs(data['enabled'], True)
        taml = stringify(data)
        self.assertIn('enabled\ttrue', taml)
    
    def test_bool_roundtrip_off(self):
        """Test 'off' parses to False and serializes back as 'false'"""
        data = parse("disabled\toff")
        self.assertIs(data['disabled'], False)
        taml = stringify(data)
        self.assertIn('disabled\tfalse', taml)
    
    def test_no_type_conversion_preserves_yes(self):
        """Test type_conversion=False keeps 'yes' as string"""
        result = parse("key\tyes", type_conversion=False)
        self.assertEqual(result['key'], 'yes')
    
    def test_all_booleans_in_one_doc(self):
        """Test all boolean variants in a single document"""
        text = "a\ttrue\nb\tYES\nc\ton\nd\tfalse\ne\tNO\nf\toff"
        result = parse(text)
        self.assertIs(result['a'], True)
        self.assertIs(result['b'], True)
        self.assertIs(result['c'], True)
        self.assertIs(result['d'], False)
        self.assertIs(result['e'], False)
        self.assertIs(result['f'], False)


class TestRawTextBlocks(unittest.TestCase):
    """Feature 2: Raw text blocks using ... syntax"""
    
    def test_basic_raw_text(self):
        """Test basic raw text block"""
        text = "message\t...\n\tHello World"
        result = parse(text)
        self.assertEqual(result['message'], 'Hello World')
    
    def test_multiline_raw_text(self):
        """Test multi-line raw text block"""
        text = "script\t...\n\tline one\n\tline two\n\tline three"
        result = parse(text)
        self.assertEqual(result['script'], 'line one\nline two\nline three')
    
    def test_raw_text_with_tabs(self):
        """Test raw text preserves tabs in content"""
        text = "message\t...\n\tHello\tWorld\twith\ttabs"
        result = parse(text)
        self.assertEqual(result['message'], 'Hello\tWorld\twith\ttabs')
    
    def test_raw_text_with_extra_indentation(self):
        """Test raw text preserves extra tabs as content"""
        text = "code\t...\n\tif true:\n\t\tdo_something()\n\tend"
        result = parse(text)
        self.assertEqual(result['code'], 'if true:\n\tdo_something()\nend')
    
    def test_raw_text_with_blank_lines(self):
        """Test raw text handles blank lines within content"""
        text = "text\t...\n\tparagraph one\n\n\tparagraph two"
        result = parse(text)
        self.assertEqual(result['text'], 'paragraph one\n\nparagraph two')
    
    def test_raw_text_ends_at_same_level(self):
        """Test raw text ends when indentation returns to same level"""
        text = "script\t...\n\techo hello\nnext_key\tvalue"
        result = parse(text)
        self.assertEqual(result['script'], 'echo hello')
        self.assertEqual(result['next_key'], 'value')
    
    def test_raw_text_empty(self):
        """Test empty raw text (no indented lines follow)"""
        text = "empty\t...\nnext\tvalue"
        result = parse(text)
        self.assertEqual(result['empty'], '')
        self.assertEqual(result['next'], 'value')
    
    def test_raw_text_in_nested_structure(self):
        """Test raw text within a nested object"""
        text = "config\n\tdatabase\n\t\thost\tlocalhost\n\tsql_query\t...\n\t\tSELECT *\n\t\tFROM users\n\tanother\tnormal"
        result = parse(text)
        self.assertEqual(result['config']['database']['host'], 'localhost')
        self.assertEqual(result['config']['sql_query'], 'SELECT *\nFROM users')
        self.assertEqual(result['config']['another'], 'normal')
    
    def test_raw_text_shell_script(self):
        """Test raw text with shell script content (from spec example)"""
        text = "script\t...\n\tif [ $1 -eq 1 ]; then\n\t\techo \"Tab indented\"\n\tfi"
        result = parse(text)
        expected = 'if [ $1 -eq 1 ]; then\n\techo "Tab indented"\nfi'
        self.assertEqual(result['script'], expected)
    
    def test_serialize_raw_text_with_newlines(self):
        """Test serializer emits raw text block for multiline strings"""
        data = {'script': 'line one\nline two'}
        result = stringify(data)
        self.assertIn('script\t...', result)
        self.assertIn('\tline one', result)
        self.assertIn('\tline two', result)
    
    def test_serialize_raw_text_with_tabs(self):
        """Test serializer emits raw text block for strings with tabs"""
        data = {'message': 'Hello\tWorld'}
        result = stringify(data)
        self.assertIn('message\t...', result)
    
    def test_raw_text_roundtrip(self):
        """Test raw text round-trip: parse → serialize → parse"""
        original_text = "script\t...\n\tif true:\n\t\tdo_thing()\n\tend"
        data = parse(original_text)
        taml = stringify(data)
        reparsed = parse(taml)
        self.assertEqual(data['script'], reparsed['script'])
    
    def test_raw_text_roundtrip_with_tabs(self):
        """Test raw text round-trip preserves tabs"""
        data = {'data': 'col1\tcol2\tcol3\nval1\tval2\tval3'}
        taml = stringify(data)
        reparsed = parse(taml)
        self.assertEqual(data['data'], reparsed['data'])
    
    def test_raw_text_sql_example(self):
        """Test raw text with SQL query (from spec)"""
        text = (
            "config\n"
            "\tsql_query\t...\n"
            "\t\tSELECT *\n"
            "\t\tFROM users u\n"
            "\t\tWHERE u.created_at > '2024-01-01'\n"
            "\t\t\tAND u.status = 'active'\n"
            "\t\tORDER BY u.name\n"
            "\tanother_setting\tnormal_value"
        )
        result = parse(text)
        expected_sql = (
            "SELECT *\n"
            "FROM users u\n"
            "WHERE u.created_at > '2024-01-01'\n"
            "\tAND u.status = 'active'\n"
            "ORDER BY u.name"
        )
        self.assertEqual(result['config']['sql_query'], expected_sql)
        self.assertEqual(result['config']['another_setting'], 'normal_value')


class TestISODateDetection(unittest.TestCase):
    """Feature 3: ISO 8601 date/time detection"""
    
    def test_date_only(self):
        """Test YYYY-MM-DD → date object"""
        result = parse("birthday\t2024-01-15")
        self.assertIsInstance(result['birthday'], date)
        self.assertEqual(result['birthday'], date(2024, 1, 15))
    
    def test_datetime_basic(self):
        """Test YYYY-MM-DDTHH:MM:SS → datetime object"""
        result = parse("timestamp\t2024-01-15T14:30:00")
        self.assertIsInstance(result['timestamp'], datetime)
        self.assertEqual(result['timestamp'], datetime(2024, 1, 15, 14, 30, 0))
    
    def test_datetime_utc(self):
        """Test YYYY-MM-DDTHH:MM:SSZ → datetime with UTC timezone"""
        result = parse("created\t2024-01-15T14:30:00Z")
        self.assertIsInstance(result['created'], datetime)
        expected = datetime(2024, 1, 15, 14, 30, 0, tzinfo=timezone.utc)
        self.assertEqual(result['created'], expected)
    
    def test_datetime_positive_offset(self):
        """Test YYYY-MM-DDTHH:MM:SS+HH:MM → datetime with timezone"""
        result = parse("tokyo\t2024-01-15T23:30:00+09:00")
        self.assertIsInstance(result['tokyo'], datetime)
        tz = timezone(timedelta(hours=9))
        expected = datetime(2024, 1, 15, 23, 30, 0, tzinfo=tz)
        self.assertEqual(result['tokyo'], expected)
    
    def test_datetime_negative_offset(self):
        """Test YYYY-MM-DDTHH:MM:SS-HH:MM → datetime with timezone"""
        result = parse("nyc\t2024-01-15T09:30:00-05:00")
        self.assertIsInstance(result['nyc'], datetime)
        tz = timezone(timedelta(hours=-5))
        expected = datetime(2024, 1, 15, 9, 30, 0, tzinfo=tz)
        self.assertEqual(result['nyc'], expected)
    
    def test_datetime_fractional_seconds(self):
        """Test YYYY-MM-DDTHH:MM:SS.sss → datetime with microseconds"""
        result = parse("precise\t2024-01-15T14:30:00.123")
        self.assertIsInstance(result['precise'], datetime)
        self.assertEqual(result['precise'].year, 2024)
        self.assertEqual(result['precise'].second, 0)
    
    def test_bare_year_stays_integer(self):
        """Test '2024' is NOT parsed as a date (stays integer)"""
        result = parse("year\t2024")
        self.assertIsInstance(result['year'], int)
        self.assertEqual(result['year'], 2024)

    def test_year_month_only(self):
        """Test YYYY-MM → date object with day=1"""
        result = parse("month\t2024-01")
        self.assertIsInstance(result['month'], date)
        self.assertNotIsInstance(result['month'], datetime)
        self.assertEqual(result['month'], date(2024, 1, 1))

    def test_year_month_december(self):
        """Test YYYY-MM at end-of-year boundary"""
        result = parse("month\t2024-12")
        self.assertEqual(result['month'], date(2024, 12, 1))

    def test_year_month_invalid_stays_string(self):
        """Test invalid YYYY-MM stays as string"""
        result = parse("bad\t2024-13")
        self.assertIsInstance(result['bad'], str)
    
    def test_invalid_date_stays_string(self):
        """Test invalid date format stays as string"""
        result = parse("not_date\t2024-13-45")
        self.assertIsInstance(result['not_date'], str)
    
    def test_date_before_number(self):
        """Test dates are detected before numbers in priority"""
        result = parse("d\t2024-01-15")
        self.assertNotIsInstance(result['d'], str)
        self.assertIsInstance(result['d'], date)
    
    def test_serialize_date(self):
        """Test date object serializes to ISO string"""
        data = {'birthday': date(2024, 1, 15)}
        result = stringify(data)
        self.assertIn('birthday\t2024-01-15', result)
    
    def test_serialize_datetime(self):
        """Test datetime object serializes to ISO string"""
        data = {'timestamp': datetime(2024, 1, 15, 14, 30, 0)}
        result = stringify(data)
        self.assertIn('timestamp\t2024-01-15T14:30:00', result)
    
    def test_serialize_datetime_utc(self):
        """Test datetime with UTC serializes with offset"""
        data = {'created': datetime(2024, 1, 15, 14, 30, 0, tzinfo=timezone.utc)}
        result = stringify(data)
        self.assertIn('2024-01-15T14:30:00', result)
    
    def test_date_roundtrip(self):
        """Test date round-trip: parse → serialize → parse"""
        text = "birthday\t2024-06-15"
        data = parse(text)
        taml = stringify(data)
        reparsed = parse(taml)
        self.assertEqual(data['birthday'], reparsed['birthday'])
    
    def test_datetime_roundtrip(self):
        """Test datetime round-trip"""
        text = "timestamp\t2024-01-15T14:30:00"
        data = parse(text)
        taml = stringify(data)
        reparsed = parse(taml)
        self.assertEqual(data['timestamp'], reparsed['timestamp'])
    
    def test_no_type_conversion_preserves_date_string(self):
        """Test type_conversion=False keeps date as string"""
        result = parse("d\t2024-01-15", type_conversion=False)
        self.assertEqual(result['d'], '2024-01-15')
        self.assertIsInstance(result['d'], str)
    
    def test_multiple_dates_in_document(self):
        """Test multiple date formats in one document"""
        text = "start\t2024-01-15\nend\t2024-12-31T23:59:59\ncreated\t2024-06-01T10:00:00Z"
        result = parse(text)
        self.assertIsInstance(result['start'], date)
        self.assertIsInstance(result['end'], datetime)
        self.assertIsInstance(result['created'], datetime)


class TestDuplicateBareKeys(unittest.TestCase):
    """Feature 4: Duplicate bare keys → collection of objects"""
    
    def test_basic_duplicate_keys(self):
        """Test two duplicate bare keys create a list of dicts"""
        text = "server\n\thost\tlocalhost\n\tport\t8080\nserver\n\thost\texample.com\n\tport\t443"
        result = parse(text)
        self.assertIsInstance(result['server'], list)
        self.assertEqual(len(result['server']), 2)
        self.assertEqual(result['server'][0]['host'], 'localhost')
        self.assertEqual(result['server'][0]['port'], 8080)
        self.assertEqual(result['server'][1]['host'], 'example.com')
        self.assertEqual(result['server'][1]['port'], 443)
    
    def test_three_duplicate_keys(self):
        """Test three duplicate bare keys"""
        text = "item\n\tname\ta\nitem\n\tname\tb\nitem\n\tname\tc"
        result = parse(text)
        self.assertIsInstance(result['item'], list)
        self.assertEqual(len(result['item']), 3)
        self.assertEqual(result['item'][0]['name'], 'a')
        self.assertEqual(result['item'][1]['name'], 'b')
        self.assertEqual(result['item'][2]['name'], 'c')
    
    def test_nested_duplicate_keys(self):
        """Test duplicate bare keys inside a parent"""
        text = "games\n\tgame\n\t\thome\tPhiladelphia\n\t\taway\tDallas\n\tgame\n\t\thome\tNew York\n\t\taway\tBoston"
        result = parse(text)
        self.assertIsInstance(result['games']['game'], list)
        self.assertEqual(len(result['games']['game']), 2)
        self.assertEqual(result['games']['game'][0]['home'], 'Philadelphia')
        self.assertEqual(result['games']['game'][1]['home'], 'New York')
    
    def test_duplicate_keys_with_mixed_siblings(self):
        """Test duplicate keys alongside regular key-value pairs"""
        text = "name\tMyApp\nserver\n\thost\ta\nserver\n\thost\tb\nversion\t1.0.0"
        result = parse(text)
        self.assertEqual(result['name'], 'MyApp')
        self.assertIsInstance(result['server'], list)
        self.assertEqual(len(result['server']), 2)
        self.assertEqual(result['version'], '1.0.0')
    
    def test_serialize_collection_of_objects(self):
        """Test serializer emits repeated parent keys for list of dicts"""
        data = {
            'server': [
                {'host': 'localhost', 'port': 8080},
                {'host': 'example.com', 'port': 443}
            ]
        }
        result = stringify(data)
        # Should contain 'server' twice (once per dict)
        self.assertEqual(result.count('\nserver\n') + result.startswith('server\n'), 2)
    
    def test_duplicate_keys_roundtrip(self):
        """Test duplicate keys round-trip: parse → serialize → parse"""
        text = "server\n\thost\tlocalhost\n\tport\t8080\nserver\n\thost\texample.com\n\tport\t443"
        data = parse(text)
        taml = stringify(data)
        reparsed = parse(taml)
        self.assertEqual(data, reparsed)
    
    def test_games_spec_example(self):
        """Test the games example from the TAML spec"""
        text = (
            "games\n"
            "\tgame\n"
            "\t\thome\tPhiladelphia\n"
            "\t\taway\tDallas\n"
            "\t\tscorehome\t120\n"
            "\t\tscoreaway\t~\n"
            "\tgame\n"
            "\t\thome\tNew York\n"
            "\t\taway\tBoston\n"
            "\t\tscorehome\t~\n"
            "\t\tscoreaway\t~"
        )
        result = parse(text)
        games = result['games']['game']
        self.assertIsInstance(games, list)
        self.assertEqual(len(games), 2)
        self.assertEqual(games[0]['home'], 'Philadelphia')
        self.assertEqual(games[0]['scorehome'], 120)
        self.assertIsNone(games[0]['scoreaway'])
        self.assertEqual(games[1]['home'], 'New York')
        self.assertIsNone(games[1]['scorehome'])
    
    def test_serialize_list_of_dicts_roundtrip(self):
        """Test full round-trip for collection of objects"""
        original = {
            'users': [
                {'name': 'Alice', 'role': 'admin'},
                {'name': 'Bob', 'role': 'user'},
                {'name': 'Charlie', 'role': 'viewer'}
            ]
        }
        taml = stringify(original)
        reparsed = parse(taml)
        self.assertEqual(reparsed['users'], original['users'])


if __name__ == '__main__':
    unittest.main()
