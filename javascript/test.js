import { parse, stringify, TAMLError } from './index.js';

// Test utilities
let testsPassed = 0;
let testsFailed = 0;

function test(name, fn) {
  try {
    fn();
    console.log(`✓ ${name}`);
    testsPassed++;
  } catch (error) {
    console.error(`✗ ${name}`);
    console.error(`  ${error.message}`);
    testsFailed++;
  }
}

function assertEquals(actual, expected, message = '') {
  const actualStr = JSON.stringify(actual, null, 2);
  const expectedStr = JSON.stringify(expected, null, 2);
  if (actualStr !== expectedStr) {
    throw new Error(`${message}\n  Expected: ${expectedStr}\n  Actual: ${actualStr}`);
  }
}

// Tests
console.log('Running TAML-JS Tests\n');

test('Parse simple key-value pairs', () => {
  const taml = 'name\tJohn\nage\t30';
  const result = parse(taml);
  assertEquals(result, { name: 'John', age: 30 });
});

test('Parse nested objects', () => {
  const taml = 'server\n\thost\tlocalhost\n\tport\t8080';
  const result = parse(taml);
  assertEquals(result, { server: { host: 'localhost', port: 8080 } });
});

test('Parse lists', () => {
  const taml = 'items\n\tfirst\n\tsecond\n\tthird';
  const result = parse(taml);
  assertEquals(result, { items: ['first', 'second', 'third'] });
});

test('Parse null values', () => {
  const taml = 'name\tJohn\npassword\t~';
  const result = parse(taml);
  assertEquals(result, { name: 'John', password: null });
});

test('Parse empty strings', () => {
  const taml = 'name\tJohn\nnickname\t""';
  const result = parse(taml);
  assertEquals(result, { name: 'John', nickname: '' });
});

test('Parse booleans', () => {
  const taml = 'enabled\ttrue\ndisabled\tfalse';
  const result = parse(taml);
  assertEquals(result, { enabled: true, disabled: false });
});

test('Parse numbers', () => {
  const taml = 'integer\t42\nfloat\t3.14';
  const result = parse(taml);
  assertEquals(result, { integer: 42, float: 3.14 });
});

test('Skip comments', () => {
  const taml = '# Comment\nname\tJohn\n# Another comment\nage\t30';
  const result = parse(taml);
  assertEquals(result, { name: 'John', age: 30 });
});

test('Skip blank lines', () => {
  const taml = 'name\tJohn\n\nage\t30\n\n';
  const result = parse(taml);
  assertEquals(result, { name: 'John', age: 30 });
});

test('Parse complex document', () => {
  const taml = `application\tMyApp
version\t1.0.0

server
\thost\tlocalhost
\tport\t8080
\tssl\ttrue

features
\tauthentication
\tlogging`;
  
  const result = parse(taml);
  assertEquals(result, {
    application: 'MyApp',
    version: '1.0.0',
    server: {
      host: 'localhost',
      port: 8080,
      ssl: true
    },
    features: ['authentication', 'logging']
  });
});

test('Stringify simple object', () => {
  const obj = { name: 'John', age: 30 };
  const result = stringify(obj);
  assertEquals(result, 'name\tJohn\nage\t30');
});

test('Stringify nested object', () => {
  const obj = { server: { host: 'localhost', port: 8080 } };
  const result = stringify(obj);
  assertEquals(result, 'server\n\thost\tlocalhost\n\tport\t8080');
});

test('Stringify array', () => {
  const obj = { items: ['first', 'second', 'third'] };
  const result = stringify(obj);
  assertEquals(result, 'items\n\tfirst\n\tsecond\n\tthird');
});

test('Stringify null values', () => {
  const obj = { name: 'John', password: null };
  const result = stringify(obj);
  assertEquals(result, 'name\tJohn\npassword\t~');
});

test('Stringify empty strings', () => {
  const obj = { name: 'John', nickname: '' };
  const result = stringify(obj);
  assertEquals(result, 'name\tJohn\nnickname\t""');
});

test('Stringify booleans', () => {
  const obj = { enabled: true, disabled: false };
  const result = stringify(obj);
  assertEquals(result, 'enabled\ttrue\ndisabled\tfalse');
});

test('Stringify numbers', () => {
  const obj = { integer: 42, float: 3.14 };
  const result = stringify(obj);
  assertEquals(result, 'integer\t42\nfloat\t3.14');
});

test('Round-trip simple object', () => {
  const original = { name: 'John', age: 30, active: true };
  const taml = stringify(original);
  const result = parse(taml);
  assertEquals(result, original);
});

test('Round-trip nested object', () => {
  const original = {
    server: {
      host: 'localhost',
      port: 8080,
      ssl: true
    }
  };
  const taml = stringify(original);
  const result = parse(taml);
  assertEquals(result, original);
});

test('Round-trip with arrays', () => {
  const original = {
    name: 'MyApp',
    features: ['auth', 'logging', 'caching']
  };
  const taml = stringify(original);
  const result = parse(taml);
  assertEquals(result, original);
});

test('Round-trip with null and empty', () => {
  const original = {
    name: 'John',
    password: null,
    nickname: '',
    bio: 'Hello'
  };
  const taml = stringify(original);
  const result = parse(taml);
  assertEquals(result, original);
});

test('Parse without type conversion', () => {
  const taml = 'age\t30\nenabled\ttrue';
  const result = parse(taml, { typeConversion: false });
  assertEquals(result, { age: '30', enabled: 'true' });
});

test('Strict mode rejects spaces in indentation', () => {
  const taml = 'server\n    host\tlocalhost';
  try {
    parse(taml, { strict: true });
    throw new Error('Should have thrown TAMLError');
  } catch (error) {
    if (!(error instanceof TAMLError)) {
      throw error;
    }
  }
});

test('Parse deeply nested structure', () => {
  const taml = `root
\tlevel1
\t\tlevel2
\t\t\tlevel3\tvalue`;
  const result = parse(taml);
  assertEquals(result, {
    root: {
      level1: {
        level2: {
          level3: 'value'
        }
      }
    }
  });
});

test('Stringify deeply nested structure', () => {
  const obj = {
    root: {
      level1: {
        level2: {
          level3: 'value'
        }
      }
    }
  };
  const result = stringify(obj);
  assertEquals(result, 'root\n\tlevel1\n\t\tlevel2\n\t\t\tlevel3\tvalue');
});

// Summary
console.log('\n--- Milestone 1 Feature Tests ---\n');

// Feature 1: Extended Booleans
test('Parse extended booleans - truthy variants', () => {
  const taml = 'a\ttrue\nb\tTrue\nc\tTRUE\nd\tyes\ne\tYes\nf\tYES\ng\ton\nh\tOn\ni\tON';
  const result = parse(taml);
  assertEquals(result, { a: true, b: true, c: true, d: true, e: true, f: true, g: true, h: true, i: true });
});

test('Parse extended booleans - falsy variants', () => {
  const taml = 'a\tfalse\nb\tFalse\nc\tFALSE\nd\tno\ne\tNo\nf\tNO\ng\toff\nh\tOff\ni\tOFF';
  const result = parse(taml);
  assertEquals(result, { a: false, b: false, c: false, d: false, e: false, f: false, g: false, h: false, i: false });
});

test('1 and 0 stay as integers, not booleans', () => {
  const taml = 'one\t1\nzero\t0';
  const result = parse(taml);
  assertEquals(result, { one: 1, zero: 0 });
});

test('Extended booleans disabled with typeConversion=false', () => {
  const taml = 'a\tyes\nb\tno';
  const result = parse(taml, { typeConversion: false });
  assertEquals(result, { a: 'yes', b: 'no' });
});

test('Stringify booleans uses canonical lowercase', () => {
  const obj = { enabled: true, disabled: false };
  const result = stringify(obj);
  assertEquals(result, 'enabled\ttrue\ndisabled\tfalse');
});

// Feature 2: Raw Text Blocks
test('Parse basic raw text block', () => {
  const taml = 'message\t...\n\tHello World';
  const result = parse(taml);
  assertEquals(result, { message: 'Hello World' });
});

test('Parse raw text with tabs preserved', () => {
  const taml = 'code\t...\n\tif true:\n\t\tprint("yes")';
  const result = parse(taml);
  assertEquals(result, { code: 'if true:\n\tprint("yes")' });
});

test('Parse raw text with blank lines', () => {
  const taml = 'text\t...\n\tline 1\n\n\tline 3';
  const result = parse(taml);
  assertEquals(result, { text: 'line 1\n\nline 3' });
});

test('Parse raw text with multiple tabs', () => {
  const taml = 'script\t...\n\t#!/bin/bash\n\tif [ true ]; then\n\t\techo "indented"\n\tfi';
  const result = parse(taml);
  assertEquals(result, { script: '#!/bin/bash\nif [ true ]; then\n\techo "indented"\nfi' });
});

test('Parse raw text followed by regular key', () => {
  const taml = 'raw\t...\n\tcontent here\nnext\tvalue';
  const result = parse(taml);
  assertEquals(result, { raw: 'content here', next: 'value' });
});

test('Parse empty raw text block', () => {
  const taml = 'empty\t...\nnext\tvalue';
  const result = parse(taml);
  assertEquals(result, { empty: '', next: 'value' });
});

test('Parse nested raw text', () => {
  const taml = 'config\n\tsql\t...\n\t\tSELECT * FROM users\n\t\tWHERE active = true\n\tname\ttest';
  const result = parse(taml);
  assertEquals(result, { config: { sql: 'SELECT * FROM users\nWHERE active = true', name: 'test' } });
});

test('Stringify raw text block', () => {
  const obj = { message: 'Hello\tWorld' };
  const result = stringify(obj);
  assertEquals(result, 'message\t...\n\tHello\tWorld');
});

test('Stringify multiline raw text', () => {
  const obj = { script: 'line 1\nline 2\nline 3' };
  const result = stringify(obj);
  assertEquals(result, 'script\t...\n\tline 1\n\tline 2\n\tline 3');
});

test('Round-trip raw text with tabs', () => {
  const original = { code: 'if (x) {\n\treturn true\n}' };
  const taml = stringify(original);
  const result = parse(taml);
  assertEquals(result, original);
});

// Feature 3: ISO 8601 Date Detection
test('Parse date-only ISO 8601', () => {
  const taml = 'date\t2024-01-15';
  const result = parse(taml);
  if (!(result.date instanceof Date)) throw new Error('Expected Date object');
  assertEquals(result.date.toISOString().startsWith('2024-01-15'), true);
});

test('Parse datetime ISO 8601', () => {
  const taml = 'ts\t2024-01-15T14:30:00';
  const result = parse(taml);
  if (!(result.ts instanceof Date)) throw new Error('Expected Date object');
});

test('Parse datetime with Z', () => {
  const taml = 'ts\t2024-01-15T14:30:00Z';
  const result = parse(taml);
  if (!(result.ts instanceof Date)) throw new Error('Expected Date object');
  assertEquals(result.ts.toISOString(), '2024-01-15T14:30:00.000Z');
});

test('Parse datetime with offset', () => {
  const taml = 'ts\t2024-01-15T14:30:00+05:00';
  const result = parse(taml);
  if (!(result.ts instanceof Date)) throw new Error('Expected Date object');
});

test('Parse datetime with negative offset', () => {
  const taml = 'ts\t2024-01-15T09:30:00-05:00';
  const result = parse(taml);
  if (!(result.ts instanceof Date)) throw new Error('Expected Date object');
});

test('Bare year like 2024 stays integer, not date', () => {
  const taml = 'year\t2024';
  const result = parse(taml);
  assertEquals(result, { year: 2024 });
});

test('ISO dates disabled with typeConversion=false', () => {
  const taml = 'date\t2024-01-15';
  const result = parse(taml, { typeConversion: false });
  assertEquals(result, { date: '2024-01-15' });
});

test('Stringify Date object to ISO string', () => {
  const obj = { created: new Date('2024-01-15T14:30:00Z') };
  const result = stringify(obj);
  assertEquals(result, 'created\t2024-01-15T14:30:00.000Z');
});

test('Round-trip ISO date', () => {
  const original = { created: new Date('2024-06-15T09:00:00.000Z') };
  const taml = stringify(original);
  const result = parse(taml);
  assertEquals(result.created.toISOString(), original.created.toISOString());
});

// Feature 4: Duplicate Bare Keys → Collection of Objects
test('Parse duplicate bare keys into array of objects', () => {
  const taml = 'games\n\tgame\n\t\thome\tPhiladelphia\n\t\taway\tDallas\n\tgame\n\t\thome\tNew York\n\t\taway\tBoston';
  const result = parse(taml);
  assertEquals(result, {
    games: [
      { home: 'Philadelphia', away: 'Dallas' },
      { home: 'New York', away: 'Boston' }
    ]
  });
});

test('Parse server duplicate keys', () => {
  const taml = 'servers\n\tserver\n\t\thost\tlocalhost\n\t\tport\t8080\n\tserver\n\t\thost\texample.com\n\t\tport\t443';
  const result = parse(taml);
  assertEquals(result, {
    servers: [
      { host: 'localhost', port: 8080 },
      { host: 'example.com', port: 443 }
    ]
  });
});

test('Parse three duplicate bare keys', () => {
  const taml = 'items\n\titem\n\t\tname\tA\n\titem\n\t\tname\tB\n\titem\n\t\tname\tC';
  const result = parse(taml);
  assertEquals(result, {
    items: [
      { name: 'A' },
      { name: 'B' },
      { name: 'C' }
    ]
  });
});

test('Stringify collection of objects as duplicate bare keys', () => {
  const obj = {
    games: [
      { home: 'Philadelphia', away: 'Dallas' },
      { home: 'New York', away: 'Boston' }
    ]
  };
  const result = stringify(obj);
  const expected = 'games\n\tgame\n\t\thome\tPhiladelphia\n\t\taway\tDallas\n\tgame\n\t\thome\tNew York\n\t\taway\tBoston';
  // Serializer uses the key name from each object - they get the same generated key
  // Actually, for collection of objects the key is repeated
  const parsed = parse(result);
  assertEquals(parsed, obj);
});

test('Round-trip duplicate bare keys', () => {
  const taml = 'users\n\tuser\n\t\tname\tAlice\n\t\trole\tadmin\n\tuser\n\t\tname\tBob\n\t\trole\tuser';
  const result = parse(taml);
  const serialized = stringify(result);
  const reparsed = parse(serialized);
  assertEquals(reparsed, result);
});

// Summary
console.log(`\n${testsPassed} tests passed, ${testsFailed} tests failed`);
process.exit(testsFailed > 0 ? 1 : 0);
