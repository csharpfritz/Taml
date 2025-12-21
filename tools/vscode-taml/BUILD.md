# Building the TAML VSCode Extension

## Quick Build

```bash
# Navigate to extension directory
cd tools/vscode-taml

# Install dependencies
npm install

# Compile TypeScript (client and server)
npm run compile

# Package the extension
npm run package

# This creates: taml-0.2.0.vsix
```

## Step-by-Step Build

### 1. Prerequisites

Install Node.js and npm (if not already installed):
- **Windows**: Download from https://nodejs.org/
- **macOS**: `brew install node`
- **Linux**: `sudo apt install nodejs npm`

Verify installation:
```bash
node --version  # Should be v16 or higher
npm --version
```

### 2. Install VSCE

VSCE (Visual Studio Code Extension Manager) is required to package extensions:

```bash
npm install -g vsce
```

Verify:
```bash
vsce --version
```

### 3. Navigate to Extension Directory

```bash
cd D:\NewTaml\tools\vscode-taml
```

Or from repo root:
```bash
cd tools/vscode-taml
```

### 4. Install Dependencies

This installs dependencies for the extension, client, and server:

```bash
npm install
```

This will automatically run `postinstall` which installs client and server dependencies.

### 5. Compile TypeScript

Compile both client and server TypeScript code:

```bash
npm run compile
```

This creates:
- `client/out/extension.js` - Language client
- `server/out/server.js` - Language server

### 6. Package Extension

```bash
npm run package
```

This will:
- Validate `package.json`
- Check all required files exist
- Bundle the extension (including compiled JS)
- Create `taml-0.2.0.vsix` file

Expected output:
```
 DONE  Packaged: D:\NewTaml\tools\vscode-taml\taml-0.2.0.vsix (XX files, X.XX KB)
```

### 7. Install in VSCode

**Option A: GUI**
1. Open VSCode
2. Go to Extensions (`Ctrl+Shift+X`)
3. Click `...` menu (top right)
4. Select "Install from VSIX..."
5. Browse to `taml-0.2.0.vsix`
6. Click "Install"

**Option B: Command Line**
```bash
code --install-extension taml-0.2.0.vsix
```

### 8. Test the Extension

1. Open VSCode
2. Create a new file: `test.taml`
3. Add some TAML content:
   ```taml
   # Test file
   app		MyApp
   version		1.0.0
   
   server
   	host		localhost
   	port		3000
   	enabled		true
   ```
4. Verify syntax highlighting works

Or open the included `test.taml` file.

## Development Mode

For active development without rebuilding:

1. Open extension folder in VSCode:
   ```bash
   cd tools/vscode-taml
   code .
   ```

2. Press `F5` to launch Extension Development Host
   - This opens a new VSCode window
   - Extension is loaded in development mode
   - Changes to grammar files take effect after reload

3. Make changes to `syntaxes/taml.tmLanguage.json`

4. In Extension Development Host window:
   - Press `Ctrl+Shift+P`
   - Type "Reload Window"
   - Press Enter

5. Test changes in `.taml` files

## Troubleshooting

### VSCE Not Found

**Error:** `'vsce' is not recognized as an internal or external command`

**Solution:**
```bash
npm install -g vsce
```

Make sure npm global bin is in your PATH.

### Package.json Validation Errors

**Error:** Missing required field

**Solution:** Check `package.json` has all required fields:
- `name`, `version`, `publisher`, `engines`, `contributes`

### Grammar Not Loading

**Error:** Syntax highlighting not working

**Solution:**
1. Check `syntaxes/taml.tmLanguage.json` exists
2. Verify `package.json` → `contributes.grammars` path is correct
3. Rebuild: `vsce package`
4. Reinstall: `code --install-extension taml-0.1.0.vsix --force`

### File Extension Not Recognized

**Error:** `.taml` files don't trigger extension

**Solution:**
1. Check `package.json` → `contributes.languages[0].extensions` includes `.taml`
2. Reload VSCode window
3. Check file actually has `.taml` extension (not `.taml.txt`)

## Publishing (Optional)

To publish to VSCode Marketplace:

### 1. Create Publisher Account
```bash
vsce create-publisher your-publisher-name
```

### 2. Get Personal Access Token
- Go to https://dev.azure.com
- User settings → Personal access tokens
- New Token with **Marketplace (Publish)** scope

### 3. Login
```bash
vsce login your-publisher-name
```

### 4. Update package.json
Change `"publisher": "taml-lang"` to your publisher name.

### 5. Publish
```bash
vsce publish
```

Or publish specific version:
```bash
vsce publish minor  # 0.1.0 → 0.2.0
vsce publish patch  # 0.1.0 → 0.1.1
vsce publish major  # 0.1.0 → 1.0.0
```

## File Structure

```
vscode-taml/
├── package.json              # Extension manifest
├── language-configuration.json  # Language config
├── syntaxes/
│   └── taml.tmLanguage.json # Grammar file
├── images/
│   ├── icon.png             # Extension icon (optional)
│   ├── taml-icon.svg        # File icon (optional)
│   └── README.md
├── README.md                # Extension documentation
├── CHANGELOG.md             # Version history
├── INSTALL.md               # Installation guide
├── BUILD.md                 # This file
├── test.taml                # Test file
├── .vscodeignore            # Files to exclude from package
└── .gitignore               # Git ignore rules
```

## Next Steps

After building:
1. Test with various TAML files
2. Try different VSCode themes
3. Test folding and comments
4. Verify tab behavior
5. Check the [examples](../../examples/) directory

## Resources

- [VSCode Extension API](https://code.visualstudio.com/api)
- [Syntax Highlight Guide](https://code.visualstudio.com/api/language-extensions/syntax-highlight-guide)
- [TextMate Grammars](https://macromates.com/manual/en/language_grammars)
- [VSCE Documentation](https://github.com/microsoft/vscode-vsce)

---

**Happy building!** 🚀
