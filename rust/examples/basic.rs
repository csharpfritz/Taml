use taml::{parse, stringify, validate, Value};
use std::collections::HashMap;

fn main() {
    println!("TAML Rust Library Examples\n");

    // Example 1: Parsing TAML
    println!("=== Example 1: Parsing TAML ===");
    let taml_text = r#"application	MyApp
version	1.0.0
author	Developer Name

server
	host	localhost
	port	8080
	ssl	true

database
	type	postgresql
	host	db.example.com
	port	5432
	password	~

features
	authentication
	logging
	caching
"#;

    match parse(taml_text) {
        Ok(value) => {
            println!("Successfully parsed TAML!");
            let obj = value.as_object().unwrap();
            println!("Application: {:?}", obj.get("application").unwrap());
            println!("Version: {:?}", obj.get("version").unwrap());
            
            if let Some(server) = obj.get("server") {
                println!("\nServer configuration:");
                let server_obj = server.as_object().unwrap();
                println!("  Host: {:?}", server_obj.get("host").unwrap());
                println!("  Port: {:?}", server_obj.get("port").unwrap());
                println!("  SSL: {:?}", server_obj.get("ssl").unwrap());
            }
            
            if let Some(features) = obj.get("features") {
                println!("\nFeatures:");
                let features_array = features.as_array().unwrap();
                for (i, feature) in features_array.iter().enumerate() {
                    println!("  {}. {:?}", i + 1, feature);
                }
            }
        }
        Err(e) => {
            eprintln!("Error parsing TAML: {}", e);
        }
    }

    // Example 2: Creating and Serializing TAML
    println!("\n=== Example 2: Creating and Serializing TAML ===");
    let mut data = HashMap::new();
    data.insert("name".to_string(), Value::String("Test App".to_string()));
    data.insert("version".to_string(), Value::String("2.0.0".to_string()));
    data.insert("enabled".to_string(), Value::Boolean(true));
    data.insert("max_connections".to_string(), Value::Integer(100));
    data.insert("timeout".to_string(), Value::Float(30.5));
    data.insert("api_key".to_string(), Value::Null);
    
    let mut config = HashMap::new();
    config.insert("debug".to_string(), Value::Boolean(true));
    config.insert("log_level".to_string(), Value::String("info".to_string()));
    data.insert("config".to_string(), Value::Object(config));
    
    let items = vec![
        Value::String("item1".to_string()),
        Value::String("item2".to_string()),
        Value::String("item3".to_string()),
    ];
    data.insert("items".to_string(), Value::Array(items));

    let taml_output = stringify(&Value::Object(data));
    println!("Generated TAML:\n{}", taml_output);

    // Example 3: Validation
    println!("\n=== Example 3: Validation ===");
    let valid_taml = "key\tvalue\nserver\n\thost\tlocalhost";
    println!("Validating valid TAML...");
    let errors = validate(valid_taml);
    if errors.is_empty() {
        println!("✓ TAML is valid!");
    } else {
        println!("✗ Found {} validation error(s)", errors.len());
    }

    let invalid_taml = "server\n    host\tlocalhost";  // spaces instead of tabs
    println!("\nValidating invalid TAML (spaces instead of tabs)...");
    let errors = validate(invalid_taml);
    if errors.is_empty() {
        println!("✓ TAML is valid!");
    } else {
        println!("✗ Found {} validation error(s):", errors.len());
        for error in errors {
            println!("  - {}", error);
        }
    }

    // Example 4: Null vs Empty String
    println!("\n=== Example 4: Null vs Empty String ===");
    let taml_with_nulls = r#"username	alice
password	~
nickname	""
bio	Software developer"#;

    match parse(taml_with_nulls) {
        Ok(value) => {
            let obj = value.as_object().unwrap();
            println!("username: {:?}", obj.get("username").unwrap());
            println!("password: {:?} (is null: {})", 
                obj.get("password").unwrap(),
                obj.get("password").unwrap().is_null());
            println!("nickname: {:?} (is empty string: {})", 
                obj.get("nickname").unwrap(),
                obj.get("nickname").unwrap().as_string() == Some(""));
            println!("bio: {:?}", obj.get("bio").unwrap());
        }
        Err(e) => {
            eprintln!("Error: {}", e);
        }
    }

    println!("\nAll examples completed!");
}
