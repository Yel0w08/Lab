# Cryptext

AES-256 file/text encryption tool for Windows.

## Usage

### File mode — drag & drop or CLI

```
Cryptext <file> [options]
```

| Argument | Description |
|---|---|
| `<file>` | File to encrypt or decrypt |
| `-key <key>` | Provide a custom encryption/decryption key |
| `-d` / `-decrypt` | Force decrypt mode |
| `-e` / `-encrypt` | Force encrypt mode |

**Encrypt a file:**
```
Cryptext secret.txt
```
→ Creates `secret.ctx` + prints the key to console.

**Decrypt a `.ctx` / `.ctxt` file:**
```
Cryptext secret.ctx
```
→ Reads the hidden `.cryptext_key` file in the same folder, creates `secret.txt`.

**Decrypt with a specific key:**
```
Cryptext secret.ctx -key "yourBase64KeyHere"
```

**Decrypt a non-standard file (`.txt` containing ciphertext):**
```
Cryptext encrypted.txt -d -key "yourBase64KeyHere"
```

**Encrypt with a custom key:**
```
Cryptext secret.txt -key "myCustomKey"
```

### Interactive mode — no arguments

```
Cryptext
```

Opens an arrow-key selector to choose Encrypt or Decrypt, then prompts for text.

## File format

- **`.ctx`** / **`.ctxt`** — Encrypted files. First 16 bytes = AES IV, rest = ciphertext. Everything is Base64-encoded.
- New output is always created next to the source file with the swapped extension (`.txt` ↔ `.ctx`).

## How it works

- **Algorithm:** AES-256-CBC with PKCS7 padding
- **Key:** 256-bit, derived from a string (UTF-8 → max 32 bytes, truncated or zero-padded)
- **Key storage (decrypt):** When you encrypt a file, a hidden `.cryptext_key` file is created in the same directory. This lets you double-click any `.ctx` file (associated with Cryptext) to decrypt it without typing a key.
- **Key storage (encrypt):** The key is always printed to console. The `.cryptext_key` file is only created when encrypting from the CLI file mode.

## Build

```
dotnet build
```
