"""Example usage of TAML Python library"""

import taml

# Example 1: Parse TAML
print("=" * 60)
print("Example 1: Parsing TAML")
print("=" * 60)

taml_text = """
# TAML Example
application	MyApp
version	1.0.0
author	Developer Name
license	~

server
	host	0.0.0.0
	port	8080
	ssl	true

database
	type	postgresql
	connection
		host	db.example.com
		port	5432
		database	myapp_db
		password	~

features
	user-authentication
	api-gateway
	rate-limiting
	logging
"""

data = taml.parse(taml_text)
print("\nParsed data:")
print(data)

# Example 2: Stringify Python object
print("\n" + "=" * 60)
print("Example 2: Serializing to TAML")
print("=" * 60)

config = {
    'application': 'TestApp',
    'version': '2.0.0',
    'debug': True,
    'server': {
        'host': 'localhost',
        'port': 3000,
        'ssl': False
    },
    'database': {
        'connection': {
            'host': '127.0.0.1',
            'port': 5432,
            'username': 'admin',
            'password': None
        }
    },
    'features': [
        'authentication',
        'caching',
        'logging'
    ]
}

taml_output = taml.stringify(config)
print("\nSerialized TAML:")
print(taml_output)

# Example 3: Round-trip
print("\n" + "=" * 60)
print("Example 3: Round-trip (parse -> stringify -> parse)")
print("=" * 60)

parsed_again = taml.parse(taml_output)
print("\nRound-trip successful:", parsed_again == config)

# Example 4: Null vs Empty String
print("\n" + "=" * 60)
print("Example 4: Null vs Empty String")
print("=" * 60)

special_values = """
username	alice
password	~
nickname	""
bio	Hello world
"""

parsed = taml.parse(special_values)
print("\nParsed special values:")
print(f"username: {repr(parsed['username'])}")
print(f"password: {repr(parsed['password'])}")
print(f"nickname: {repr(parsed['nickname'])}")
print(f"bio: {repr(parsed['bio'])}")

# Example 5: Type conversion
print("\n" + "=" * 60)
print("Example 5: Type Conversion")
print("=" * 60)

typed_taml = """
string	hello
integer	42
float	3.14159
bool_true	true
bool_false	false
null_value	~
empty_string	""
"""

with_types = taml.parse(typed_taml, type_conversion=True)
without_types = taml.parse(typed_taml, type_conversion=False)

print("\nWith type conversion:")
for key, value in with_types.items():
    print(f"  {key}: {repr(value)} ({type(value).__name__})")

print("\nWithout type conversion:")
for key, value in without_types.items():
    print(f"  {key}: {repr(value)} ({type(value).__name__})")
