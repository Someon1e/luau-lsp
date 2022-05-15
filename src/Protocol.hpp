#pragma once
#include <string>
#include <optional>
#include <variant>

#include "Uri.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace lsp
{
using URI = Uri;
using DocumentUri = Uri;

enum struct ErrorCode
{
    // JSON RPC errors
    ParseError = -32700,
    InvalidRequest = -32600,
    MethodNotFound = -32601,
    InvalidParams = -32602,
    InternalError = -32603,

    // LSP Errors
    ServerNotInitialized = -32002,
    UnknownErrorCode = -32001,
    RequestFailed = -32803,
    ServerCancelled = -32802,
    ContentModified = -32801,
    RequestCancelled = -32800,
};

struct ClientCapabilities
{
    // TODO
};
void to_json(json&, const ClientCapabilities&){};
void from_json(const json&, ClientCapabilities&){};

struct WorkspaceFolder
{
    DocumentUri uri;
    std::string name;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WorkspaceFolder, uri, name);

enum struct TraceValue
{
    Off,
    Messages,
    Verbose,
};
NLOHMANN_JSON_SERIALIZE_ENUM(TraceValue, {
                                             {TraceValue::Off, "off"},
                                             {TraceValue::Messages, "messages"},
                                             {TraceValue::Verbose, "verbose"},
                                         });

struct InitializeParams
{
    // struct ClientInfo
    // {
    //     std::string name;
    //     std::optional<std::string> version;
    // };

    // std::optional<int> processId;
    // std::optional<ClientInfo> clientInfo;
    // std::optional<std::string> locale;
    // rootPath
    std::optional<DocumentUri> rootUri; // TODO: this is nullable!
    // std::optional<json> initializationOptions;
    // ClientCapabilities capabilities;
    TraceValue trace = TraceValue::Off;
    std::optional<std::vector<WorkspaceFolder>> workspaceFolders;
    // workspaceFolders
};

void from_json(const json& j, InitializeParams& p)
{
    if (!j.contains("rootUri") || j.at("rootUri").is_null())
    {
        p.rootUri = std::nullopt;
    }
    else
    {
        p.rootUri = j.at("rootUri").get<DocumentUri>();
    }
    if (j.contains("trace"))
        p.trace = j.at("trace").get<TraceValue>();

    if (j.contains("workspaceFolders"))
    {
        if (j.at("workspaceFolders").is_null())
        {
            p.workspaceFolders = std::nullopt;
        }
        else
        {
            p.workspaceFolders = j.at("workspaceFolders").get<std::vector<WorkspaceFolder>>();
        }
    }
}

struct Registration
{
    std::string id;
    std::string method;
    json registerOptions = nullptr;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Registration, id, method, registerOptions);

struct RegistrationParams
{
    std::vector<Registration> registrations;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RegistrationParams, registrations);

struct CompletionOptions
{
    std::optional<std::vector<std::string>> triggerCharacters;
    std::optional<std::vector<std::string>> allCommitCharacters;
    bool resolveProvider = false;

    struct CompletionItem
    {
        bool labelDetailsSupport = false;
    };
    std::optional<CompletionItem> completionItem;
};
void to_json(json& j, const CompletionOptions& p)
{
    j = json{{"resolveProvider", p.resolveProvider}};
    if (p.triggerCharacters)
        j["triggerCharacters"] = p.triggerCharacters.value();
    if (p.allCommitCharacters)
        j["allCommitCharacters"] = p.allCommitCharacters.value();
    if (p.completionItem)
    {
        j["completionItem"] = {};
        j["completionItem"]["labelDetailsSupport"] = p.completionItem->labelDetailsSupport;
    }
}

struct DocumentLinkOptions
{
    bool resolveProvider = false;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DocumentLinkOptions, resolveProvider);

struct SignatureHelpOptions
{
    std::optional<std::vector<std::string>> triggerCharacters;
    std::optional<std::vector<std::string>> retriggerCharacters;
};
void to_json(json& j, const SignatureHelpOptions& p)
{
    j = json{};
    if (p.triggerCharacters)
        j["triggerCharacters"] = p.triggerCharacters.value();
    if (p.retriggerCharacters)
        j["retriggerCharacters"] = p.retriggerCharacters.value();
}

enum struct TextDocumentSyncKind
{
    None = 0,
    Full = 1,
    Incremental = 2,
};

struct WorkspaceFoldersServerCapabilities
{
    bool supported = false;
    bool changeNotifications = false;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WorkspaceFoldersServerCapabilities, supported, changeNotifications);

struct WorkspaceCapabilities
{
    std::optional<WorkspaceFoldersServerCapabilities> workspaceFolders;
    // fileOperations
};
void to_json(json& j, const WorkspaceCapabilities& p)
{
    j = {};
    if (p.workspaceFolders.has_value())
        j["workspaceFolders"] = p.workspaceFolders.value();
}

struct ServerCapabilities
{
    std::optional<TextDocumentSyncKind> textDocumentSync;
    std::optional<CompletionOptions> completionProvider;
    std::optional<DocumentLinkOptions> documentLinkProvider;
    bool hoverProvider = false;
    std::optional<SignatureHelpOptions> signatureHelpProvider;
    std::optional<WorkspaceCapabilities> workspace;
};

void to_json(json& j, const ServerCapabilities& p)
{
    j = json{{"hoverProvider", p.hoverProvider}};
    if (p.textDocumentSync)
        j["textDocumentSync"] = p.textDocumentSync.value();
    if (p.completionProvider)
        j["completionProvider"] = p.completionProvider.value();
    if (p.documentLinkProvider)
        j["documentLinkProvider"] = p.documentLinkProvider.value();
    if (p.signatureHelpProvider)
        j["signatureHelpProvider"] = p.signatureHelpProvider.value();
    if (p.workspace)
        j["workspace"] = p.workspace.value();
}

struct InitializeResult
{
    struct ServerInfo
    {
        std::string name;
        std::optional<std::string> version;
    };

    ServerCapabilities capabilities;
    std::optional<ServerInfo> serverInfo;
};

void to_json(json& j, const InitializeResult& p)
{
    j = json{
        {"capabilities", p.capabilities},
    };
    if (p.serverInfo)
    {
        j["serverInfo"] = {{"name", p.serverInfo->name}};
        if (p.serverInfo->version)
            j["serverInfo"] = {{"name", p.serverInfo->name}, {"version", p.serverInfo->version.value()}};
    }
}

struct InitializedParams
{
};
void to_json(json&, const InitializedParams&){};
void from_json(const json&, InitializedParams&){};

struct Position
{
    size_t line;
    size_t character;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Position, line, character);

struct Range
{
    Position start;
    Position end;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Range, start, end);

struct TextDocumentItem
{
    DocumentUri uri;
    std::string languageId;
    int version;
    std::string text;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TextDocumentItem, uri, languageId, version, text);

struct TextDocumentIdentifier
{
    DocumentUri uri;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TextDocumentIdentifier, uri);

struct VersionedTextDocumentIdentifier : TextDocumentIdentifier
{
    int version;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(VersionedTextDocumentIdentifier, uri, version);

struct TextDocumentPositionParams
{
    TextDocumentIdentifier textDocument;
    Position position;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TextDocumentPositionParams, textDocument, position);

struct TextEdit
{
    Range range;
    std::string newText;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TextEdit, range, newText);

struct Location
{
    DocumentUri uri;
    Range range;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Location, uri, range);

struct Command
{
    std::string title;
    std::string command;
    std::vector<json> arguments;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Command, title, command, arguments);

struct DidOpenTextDocumentParams
{
    TextDocumentItem textDocument;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DidOpenTextDocumentParams, textDocument);

struct TextDocumentContentChangeEvent
{
    // If only text is provided, then its considered to be the whole document
    std::optional<Range> range;
    std::string text;
};
void from_json(const json& j, TextDocumentContentChangeEvent& p)
{
    j.at("text").get_to(p.text);
    if (j.contains("range"))
        p.range = j.at("range").get<Range>();
}
void to_json(json& j, const TextDocumentContentChangeEvent& p)
{
    j = json{
        {"text", p.text},
    };
    if (p.range)
        j["range"] = p.range.value();
}

struct DidChangeTextDocumentParams
{
    VersionedTextDocumentIdentifier textDocument;
    std::vector<TextDocumentContentChangeEvent> contentChanges;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DidChangeTextDocumentParams, textDocument, contentChanges);

struct DidCloseTextDocumentParams
{
    TextDocumentIdentifier textDocument;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DidCloseTextDocumentParams, textDocument);

using Pattern = std::string;
using GlobPattern = Pattern; // | RelativePattern

enum WatchKind
{
    Create = 1,
    Change = 2,
    Delete = 4,
};

struct FileSystemWatcher
{
    GlobPattern globPattern;
    int kind = WatchKind::Create | WatchKind::Change | WatchKind::Delete;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FileSystemWatcher, globPattern, kind);

struct DidChangeWatchedFilesRegistrationOptions
{
    std::vector<FileSystemWatcher> watchers;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DidChangeWatchedFilesRegistrationOptions, watchers);

enum struct FileChangeType
{
    Created = 1,
    Changed = 2,
    Deleted = 3,
};

struct FileEvent
{
    DocumentUri uri;
    FileChangeType type;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FileEvent, uri, type);

struct DidChangeWatchedFilesParams
{
    std::vector<FileEvent> changes;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DidChangeWatchedFilesParams, changes);

enum struct DiagnosticSeverity
{
    Error = 1,
    Warning = 2,
    Information = 3,
    Hint = 4,
};

enum struct DiagnosticTag
{
    Unnecessary = 1,
    Deprecated = 2,
};

struct DiagnosticRelatedInformation
{
    Location location;
    std::string message;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DiagnosticRelatedInformation, location, message);

struct CodeDescription
{
    URI href;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CodeDescription, href);

struct Diagnostic
{
    Range range;
    std::optional<DiagnosticSeverity> severity;
    std::optional<std::variant<std::string, int>> code;
    std::optional<CodeDescription> codeDescription;
    std::optional<std::string> source;
    std::string message;
    std::optional<std::vector<DiagnosticTag>> tags;
    std::optional<std::vector<DiagnosticRelatedInformation>> relatedInformation;
    // data?
};
void to_json(json& j, const Diagnostic& p)
{
    j = json{{"range", p.range}, {"message", p.message}};
    if (p.severity)
        j["severity"] = p.severity.value();
    if (p.code)
    {
        if (std::holds_alternative<int>(p.code.value()))
        {
            j["code"] = std::get<int>(p.code.value());
        }
        else
        {
            j["code"] = std::get<std::string>(p.code.value());
        }
    }
    if (p.codeDescription)
        j["codeDescription"] = p.codeDescription.value();
    if (p.source)
        j["source"] = p.source.value();
    if (p.tags)
        j["tags"] = p.tags.value();
    if (p.relatedInformation)
        j["relatedInformation"] = p.relatedInformation.value();
}

struct PublishDiagnosticsParams
{
    DocumentUri uri;
    std::optional<int> version;
    std::vector<Diagnostic> diagnostics;
};
void to_json(json& j, const PublishDiagnosticsParams& p)
{
    j = json{{"uri", p.uri}, {"diagnostics", p.diagnostics}};
    if (p.version)
        j["version"] = p.version.value();
}

enum struct CompletionTriggerKind
{
    Invoked = 1,
    TriggerCharacter = 2,
    TriggerForIncompleteCompletions = 3
};

struct CompletionContext
{
    CompletionTriggerKind triggerKind;
    std::optional<std::string> triggerCharacter;
};

struct CompletionParams : TextDocumentPositionParams
{
    std::optional<CompletionContext> context;
};

enum struct InsertTextFormat
{
    PlainText = 1,
    Snippet = 2,
};

enum struct CompletionItemTag
{
    Deprecated = 1,
};

enum struct InsertTextMode
{
    AsIs = 1,
    AdjustIndentation = 2,
};

struct CompletionItemLabelDetails
{
    std::optional<std::string> detail;
    std::optional<std::string> description;
};
void to_json(json& j, const CompletionItemLabelDetails& p)
{
    j = json{};
    if (p.detail)
        j["detail"] = p.detail.value();
    if (p.description)
        j["detail"] = p.description.value();
}

enum struct CompletionItemKind
{
    Text = 1,
    Method = 2,
    Function = 3,
    Constructor = 4,
    Field = 5,
    Variable = 6,
    Class = 7,
    Interface = 8,
    Module = 9,
    Property = 10,
    Unit = 11,
    Value = 12,
    Enum = 13,
    Keyword = 14,
    Snippet = 15,
    Color = 16,
    File = 17,
    Reference = 18,
    Folder = 19,
    EnumMember = 20,
    Constant = 21,
    Struct = 22,
    Event = 23,
    Operator = 24,
    TypeParameter = 25,
};

struct CompletionItem
{
    std::string label;
    std::optional<CompletionItemLabelDetails> labelDetails;
    std::optional<CompletionItemKind> kind;
    // std::optional<std::vector<CompletionItemTag>> tags;
    std::optional<std::string> detail;
    std::optional<std::string> documentation;
    bool deprecated = false;
    // bool preselect = false;
    // std::optional<std::string> sortText;
    // std::optional<std::string> filterText;
    std::optional<std::string> insertText;
    InsertTextFormat insertTextFormat = InsertTextFormat::PlainText;
    // std::optional<InsertTextMode> insertTextMode;
    // std::optional<TextEdit> textEdit;
    // std::optional<std::string> textEditString;
    // std::optional<std::vector<TextEdit>> additionalTextEdits;
    // std::optional<std::vector<std::string>> commitCharacters;
    std::optional<Command> command;
    // data?
};
void to_json(json& j, const CompletionItem& p)
{
    j = json{{"label", p.label}, {"deprecated", p.deprecated}, {"insertTextFormat", p.insertTextFormat}};
    if (p.kind)
        j["kind"] = p.kind.value();
    if (p.insertText)
        j["insertText"] = p.insertText.value();
    if (p.detail)
        j["detail"] = p.detail.value();
    if (p.labelDetails)
        j["labelDetails"] = p.labelDetails.value();
    if (p.documentation)
        j["documentation"] = p.documentation.value();
    if (p.command)
        j["command"] = p.command.value();
}

struct DocumentLinkParams
{
    TextDocumentIdentifier textDocument;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DocumentLinkParams, textDocument);

struct DocumentLink
{
    Range range;
    DocumentUri target; // TODO: potentially optional if we resolve later
    // std::optional<std::string> tooltip;
    // std::optional<json> data; // for resolver
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DocumentLink, range, target);

struct HoverParams : TextDocumentPositionParams
{
};

enum struct MarkupKind
{
    PlainText,
    Markdown,
};
NLOHMANN_JSON_SERIALIZE_ENUM(MarkupKind, {{MarkupKind::PlainText, "plaintext"}, {MarkupKind::Markdown, "markdown"}});

struct MarkupContent
{
    MarkupKind kind;
    std::string value;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MarkupContent, kind, value);

struct Hover
{
    MarkupContent contents;
    std::optional<Range> range;
};
void to_json(json& j, const Hover& p)
{
    j = {{"contents", p.contents}};
    if (p.range)
        j["range"] = p.range.value();
}

struct ParameterInformation
{
    std::string label;
    std::optional<MarkupContent> documentation;
};
void to_json(json& j, const ParameterInformation& p)
{
    j = {{"label", p.label}};
    if (p.documentation)
        j["documentation"] = p.documentation.value();
}
void from_json(const json& j, ParameterInformation& p)
{
    j.at("label").get_to(p.label);
    if (j.contains("documentation"))
        p.documentation = j.at("documentation").get<MarkupContent>();
}

struct SignatureInformation
{
    std::string label;
    std::optional<MarkupContent> documentation;
    std::optional<std::vector<ParameterInformation>> parameters;
    std::optional<size_t> activeParameter;
};
void to_json(json& j, const SignatureInformation& p)
{
    j = {{"label", p.label}};
    if (p.documentation)
        j["documentation"] = p.documentation.value();
    if (p.parameters)
        j["parameters"] = p.parameters.value();
    if (p.activeParameter)
        j["activeParameter"] = p.activeParameter.value();
}
void from_json(const json& j, SignatureInformation& p)
{
    j.at("label").get_to(p.label);
    if (j.contains("documentation"))
        p.documentation = j.at("documentation").get<MarkupContent>();
    if (j.contains("parameters"))
        p.parameters = j.at("parameters").get<std::vector<ParameterInformation>>();
    if (j.contains("activeParameter"))
        p.activeParameter = j.at("activeParameter").get<size_t>();
}

struct SignatureHelp
{
    std::vector<SignatureInformation> signatures;
    size_t activeSignature = 0;
    size_t activeParameter = 0;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SignatureHelp, signatures, activeSignature, activeParameter);

enum struct SignatureHelpTriggerKind
{
    Invoked = 1,
    TriggerCharacter = 2,
    ContentChange = 3,
};

struct SignatureHelpContext
{
    SignatureHelpTriggerKind triggerKind;
    std::optional<std::string> triggerCharacter;
    bool isRetrigger;
    std::optional<SignatureHelp> activeSignatureHelp;
};
void from_json(const json& j, SignatureHelpContext& p)
{
    j.at("triggerKind").get_to(p.triggerKind);
    j.at("isRetrigger").get_to(p.isRetrigger);
    if (j.contains("triggerCharacter"))
        p.triggerCharacter = j.at("triggerCharacter").get<std::string>();
    if (j.contains("activeSignatureHelp"))
        p.activeSignatureHelp = j.at("activeSignatureHelp").get<SignatureHelp>();
}

struct SignatureHelpParams : TextDocumentPositionParams
{
    std::optional<SignatureHelpContext> context;
};

struct WorkspaceFoldersChangeEvent
{
    std::vector<WorkspaceFolder> added;
    std::vector<WorkspaceFolder> removed;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WorkspaceFoldersChangeEvent, added, removed);

struct DidChangeWorkspaceFoldersParams
{
    WorkspaceFoldersChangeEvent event;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DidChangeWorkspaceFoldersParams, event);

struct SetTraceParams
{
    TraceValue value;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SetTraceParams, value);

enum struct MessageType
{
    Error = 1,
    Warning = 2,
    Info = 3,
    Log = 4,
};

struct LogMessageParams
{
    MessageType type;
    std::string message;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LogMessageParams, type, message);

struct ShowMessageParams
{
    MessageType type;
    std::string message;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ShowMessageParams, type, message);

} // namespace lsp