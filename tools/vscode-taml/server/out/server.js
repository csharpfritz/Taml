"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const node_1 = require("vscode-languageserver/node");
const vscode_languageserver_textdocument_1 = require("vscode-languageserver-textdocument");
const connection = (0, node_1.createConnection)(node_1.ProposedFeatures.all);
const documents = new node_1.TextDocuments(vscode_languageserver_textdocument_1.TextDocument);
let hasConfigurationCapability = false;
connection.onInitialize((params) => {
    const capabilities = params.capabilities;
    hasConfigurationCapability = !!(capabilities.workspace && !!capabilities.workspace.configuration);
    const result = {
        capabilities: {
            textDocumentSync: node_1.TextDocumentSyncKind.Incremental
        }
    };
    return result;
});
connection.onInitialized(() => {
    if (hasConfigurationCapability) {
        connection.client.register(node_1.DidChangeConfigurationNotification.type, undefined);
    }
});
const defaultSettings = { validation: { enable: true, showWarnings: true } };
let globalSettings = defaultSettings;
connection.onDidChangeConfiguration(change => {
    globalSettings = (change.settings.taml || defaultSettings);
    documents.all().forEach(validateTextDocument);
});
documents.onDidChangeContent(change => {
    validateTextDocument(change.document);
});
async function validateTextDocument(textDocument) {
    const diagnostics = [];
    const lines = textDocument.getText().split(/\r?\n/);
    let previousLine = { indentLevel: -1, isParent: false };
    for (let i = 0; i < lines.length; i++) {
        const line = lines[i];
        if (!line.trim() || line.trimStart().startsWith('#')) {
            previousLine = { indentLevel: previousLine.indentLevel, isParent: false };
            continue;
        }
        const lineInfo = validateLine(line, i, previousLine, diagnostics, globalSettings.validation.showWarnings);
        previousLine = lineInfo;
    }
    connection.sendDiagnostics({ uri: textDocument.uri, diagnostics });
}
function validateLine(line, lineNumber, previousLine, diagnostics, showWarnings) {
    const lineInfo = { indentLevel: 0, isParent: false };
    if (line.length > 0 && line[0] === ' ') {
        diagnostics.push({ severity: node_1.DiagnosticSeverity.Error, range: { start: { line: lineNumber, character: 0 }, end: { line: lineNumber, character: 1 } }, message: 'Indentation must use tabs, not spaces', source: 'taml' });
        return lineInfo;
    }
    let indentLevel = 0;
    for (let i = 0; i < line.length; i++) {
        if (line[i] === '\t')
            indentLevel++;
        else if (line[i] === ' ') {
            diagnostics.push({ severity: node_1.DiagnosticSeverity.Error, range: { start: { line: lineNumber, character: i }, end: { line: lineNumber, character: i + 1 } }, message: 'Mixed spaces and tabs in indentation', source: 'taml' });
            return lineInfo;
        }
        else
            break;
    }
    lineInfo.indentLevel = indentLevel;
    if (previousLine.indentLevel >= 0) {
        if (indentLevel > previousLine.indentLevel + 1) {
            diagnostics.push({ severity: node_1.DiagnosticSeverity.Error, range: { start: { line: lineNumber, character: 0 }, end: { line: lineNumber, character: indentLevel } }, message: `Invalid indentation level (expected max ${previousLine.indentLevel + 1} tabs, found ${indentLevel})`, source: 'taml' });
        }
        if (indentLevel > previousLine.indentLevel && !previousLine.isParent) {
            diagnostics.push({ severity: node_1.DiagnosticSeverity.Error, range: { start: { line: lineNumber, character: 0 }, end: { line: lineNumber, character: indentLevel } }, message: 'Indented line has no parent (previous line was not a parent key)', source: 'taml' });
        }
    }
    const content = line.substring(indentLevel);
    if (!content.trim()) {
        diagnostics.push({ severity: node_1.DiagnosticSeverity.Error, range: { start: { line: lineNumber, character: indentLevel }, end: { line: lineNumber, character: line.length } }, message: 'Line has no content after indentation', source: 'taml' });
        return lineInfo;
    }
    const firstTabIndex = content.indexOf('\t');
    if (firstTabIndex === -1) {
        lineInfo.isParent = true;
        if (showWarnings && content.includes('  ')) {
            const idx = content.indexOf('  ');
            diagnostics.push({ severity: node_1.DiagnosticSeverity.Warning, range: { start: { line: lineNumber, character: indentLevel + idx }, end: { line: lineNumber, character: indentLevel + idx + 2 } }, message: 'Key contains multiple spaces (did you mean to use tabs?)', source: 'taml' });
        }
    }
    else if (firstTabIndex === 0) {
        diagnostics.push({ severity: node_1.DiagnosticSeverity.Error, range: { start: { line: lineNumber, character: indentLevel }, end: { line: lineNumber, character: indentLevel + 1 } }, message: 'Key is empty (line starts with tab)', source: 'taml' });
    }
    else {
        lineInfo.isParent = false;
        let valueStart = firstTabIndex;
        while (valueStart < content.length && content[valueStart] === '\t')
            valueStart++;
        if (valueStart < content.length) {
            const value = content.substring(valueStart);
            if (value.includes('\t')) {
                const tabIdx = value.indexOf('\t');
                diagnostics.push({ severity: node_1.DiagnosticSeverity.Error, range: { start: { line: lineNumber, character: indentLevel + valueStart + tabIdx }, end: { line: lineNumber, character: indentLevel + valueStart + tabIdx + 1 } }, message: 'Value contains invalid tab character', source: 'taml' });
            }
        }
    }
    return lineInfo;
}
documents.listen(connection);
connection.listen();
//# sourceMappingURL=server.js.map