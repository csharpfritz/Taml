import sys
sys.path.insert(0, '.')
from taml import parse, stringify

# Test 1: Simple parsing
print('Test 1: Simple parsing')
text = 'name' + chr(9) + 'value'
result = parse(text)
print(f'Result: {result}')
assert result == {'name': 'value'}, 'Failed'
print('PASS')

# Test 2: Simple stringify
print('\nTest 2: Simple stringify')
data = {'name': 'value', 'key': 'data'}
result = stringify(data)
print(f'Result: {repr(result)}')
assert 'name' + chr(9) + 'value' in result, 'Failed'
print('PASS')

# Test 3: Nested structure
print('\nTest 3: Nested structure')
text = 'parent\n' + chr(9) + 'child' + chr(9) + 'value'
result = parse(text)
print(f'Result: {result}')
assert result == {'parent': {'child': 'value'}}, 'Failed'
print('PASS')

# Test 4: List
print('\nTest 4: List')
text = 'items\n' + chr(9) + 'first\n' + chr(9) + 'second'
result = parse(text)
print(f'Result: {result}')
assert result == {'items': ['first', 'second']}, 'Failed'
print('PASS')

# Test 5: Null and empty
print('\nTest 5: Null and empty')
text = 'null_val' + chr(9) + '~\nempty' + chr(9) + '""'
result = parse(text)
print(f'Result: {result}')
assert result['null_val'] is None, 'Failed null'
assert result['empty'] == '', 'Failed empty'
print('PASS')

print('\n✓ All basic tests passed!')
