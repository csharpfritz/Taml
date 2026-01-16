use taml::{parse, stringify, validate};
use std::fs;

#[test]
fn test_parse_and_stringify_roundtrip() {
    let taml = r#"application	MyApp
version	1.0.0

server
	host	localhost
	port	8080
	ssl	true

database
	type	postgresql
	connection
		host	db.example.com
		port	5432

features
	authentication
	logging
	caching"#;

    // Parse
    let parsed = parse(taml).expect("Failed to parse TAML");
    
    // Verify structure
    let obj = parsed.as_object().expect("Root should be an object");
    assert_eq!(obj.get("application").unwrap().as_string(), Some("MyApp"));
    assert_eq!(obj.get("version").unwrap().as_string(), Some("1.0.0"));
    
    let server = obj.get("server").unwrap().as_object().unwrap();
    assert_eq!(server.get("host").unwrap().as_string(), Some("localhost"));
    assert_eq!(server.get("port").unwrap().as_integer(), Some(8080));
    assert_eq!(server.get("ssl").unwrap().as_boolean(), Some(true));
    
    let features = obj.get("features").unwrap().as_array().unwrap();
    assert_eq!(features.len(), 3);
    assert_eq!(features[0].as_string(), Some("authentication"));
    
    // Serialize
    let serialized = stringify(&parsed);
    
    // Parse again
    let reparsed = parse(&serialized).expect("Failed to reparse");
    
    // Basic verification that structure is maintained
    let reparsed_obj = reparsed.as_object().unwrap();
    assert_eq!(reparsed_obj.get("application").unwrap().as_string(), Some("MyApp"));
}

#[test]
fn test_null_and_empty_values() {
    let taml = r#"name	Alice
password	~
nickname	""
bio	Hello world"#;

    let parsed = parse(taml).unwrap();
    let obj = parsed.as_object().unwrap();
    
    assert_eq!(obj.get("name").unwrap().as_string(), Some("Alice"));
    assert!(obj.get("password").unwrap().is_null());
    assert_eq!(obj.get("nickname").unwrap().as_string(), Some(""));
    assert_eq!(obj.get("bio").unwrap().as_string(), Some("Hello world"));
}

#[test]
fn test_nested_structures() {
    let taml = r#"config
	database
		host	localhost
		port	5432
		credentials
			username	admin
			password	secret
	features
		auth
		logging
		caching"#;

    let parsed = parse(taml).unwrap();
    let obj = parsed.as_object().unwrap();
    
    let config = obj.get("config").unwrap().as_object().unwrap();
    let database = config.get("database").unwrap().as_object().unwrap();
    let credentials = database.get("credentials").unwrap().as_object().unwrap();
    
    assert_eq!(credentials.get("username").unwrap().as_string(), Some("admin"));
    assert_eq!(credentials.get("password").unwrap().as_string(), Some("secret"));
    
    let features = config.get("features").unwrap().as_array().unwrap();
    assert_eq!(features.len(), 3);
}

#[test]
fn test_validation_valid_taml() {
    let taml = r#"key	value
server
	host	localhost
	port	8080"#;

    let errors = validate(taml);
    assert_eq!(errors.len(), 0, "Valid TAML should have no errors");
}

#[test]
fn test_validation_space_indentation() {
    let taml = "server\n    host\tlocalhost";
    let errors = validate(taml);
    assert!(errors.len() > 0, "Should detect space indentation");
}

#[test]
fn test_validation_tab_in_value() {
    let taml = "key\tvalue\twith\ttabs";
    let errors = validate(taml);
    assert!(errors.len() > 0, "Should detect tabs in value");
}

#[test]
fn test_type_conversion() {
    let taml = r#"string	hello
integer	42
float	3.14
bool_true	true
bool_false	false
null_value	~"#;

    let parsed = parse(taml).unwrap();
    let obj = parsed.as_object().unwrap();
    
    assert!(obj.get("string").unwrap().is_string());
    assert!(obj.get("integer").unwrap().is_integer());
    assert!(obj.get("float").unwrap().is_float());
    assert!(obj.get("bool_true").unwrap().is_boolean());
    assert!(obj.get("bool_false").unwrap().is_boolean());
    assert!(obj.get("null_value").unwrap().is_null());
}

#[test]
fn test_comments_and_empty_lines() {
    let taml = r#"# This is a comment
application	MyApp

# Another comment
version	1.0.0

# Server configuration
server
	host	localhost"#;

    let parsed = parse(taml).unwrap();
    let obj = parsed.as_object().unwrap();
    
    assert_eq!(obj.get("application").unwrap().as_string(), Some("MyApp"));
    assert_eq!(obj.get("version").unwrap().as_string(), Some("1.0.0"));
}

#[test]
fn test_parse_example_file() {
    // Try to load and parse web-app-config.taml example
    let example_path = "../examples/web-app-config.taml";
    if let Ok(content) = fs::read_to_string(example_path) {
        let parsed = parse(&content);
        assert!(parsed.is_ok(), "Should parse example file successfully: {:?}", parsed.err());
        
        if let Ok(value) = parsed {
            let obj = value.as_object().unwrap();
            // Verify a few fields from the example
            assert!(obj.get("application").is_some());
            assert!(obj.get("server").is_some());
        }
    }
}
