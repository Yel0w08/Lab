using System;
using System.IO;
using System.Security.Cryptography;
using System.Text;

string encryptionMethod = "AES256";
string Key = null;

Main();

void Main()
{
    Console.ForegroundColor = ConsoleColor.Red;
    Console.WriteLine("Cryptext");
    Console.ForegroundColor = ConsoleColor.White;

    string[] args = Environment.GetCommandLineArgs();
    string inputFile = null;
    bool forceDecrypt = false;
    bool forceEncrypt = false;

    for (int i = 1; i < args.Length; i++)
    {
        if (args[i] == "-key" && i + 1 < args.Length)
        {
            Key = args[++i];
        }
        else if (args[i] == "-decrypt" || args[i] == "-d")
        {
            forceDecrypt = true;
        }
        else if (args[i] == "-encrypt" || args[i] == "-e")
        {
            forceEncrypt = true;
        }
        else if (!args[i].StartsWith("-") && inputFile == null)
        {
            inputFile = args[i];
        }
    }

    if (inputFile != null && File.Exists(inputFile))
    {
        string ext = Path.GetExtension(inputFile).ToLowerInvariant();
        bool isCtx = ext == ".ctx" || ext == ".ctxt";

        if (forceEncrypt) isCtx = false;
        if (forceDecrypt) isCtx = true;

        string content = File.ReadAllText(inputFile).Trim();

        if (isCtx)
        {
            if (Key == null)
            {
                string inputDir = Path.GetDirectoryName(Path.GetFullPath(inputFile)) ?? ".";
                string keyPath = Path.Combine(inputDir, ".cryptext_key");
                if (File.Exists(keyPath))
                    Key = File.ReadAllText(keyPath).Trim();
                else
                {
                    Console.WriteLine("Error: no key provided. Use -key <key>.");
                    Wait();
                    return;
                }
            }

            string result = Decrypt(content, Key);
            if (result.StartsWith("Error:"))
            {
                Console.WriteLine(result);
                Wait();
                return;
            }

            string outputPath = Path.ChangeExtension(inputFile, ".txt");
            File.WriteAllText(outputPath, result);
            Console.WriteLine($"Decrypted -> {outputPath}");
        }
        else
        {
            if (Key == null)
                Key = generateKey();

            string result = Encrypt(content, Key);
            if (result.StartsWith("Error:"))
            {
                Console.WriteLine(result);
                Wait();
                return;
            }

            string outputPath = Path.ChangeExtension(inputFile, ".ctx");
            File.WriteAllText(outputPath, result);
            Console.WriteLine($"Encrypted -> {outputPath}");
            Console.WriteLine($"Key: {Key}");
        }

        Wait();
        return;
    }

    Console.WriteLine("No file assigned, using manual mode");

    bool encryptMode = SelectMode();

    Console.Clear();
    Console.WriteLine("The text you want to " + (encryptMode ? "encrypt" : "decrypt"));
    Console.Write("\n\n--> ");
    string userInput = Console.ReadLine();

    if (encryptMode)
    {
        Console.Clear();
        Console.WriteLine($"Encryption Algoritm : {encryptionMethod}");
        Console.WriteLine($"Encryption key      : {(Key == null ? "Automatic" : "Manual")}");
        Console.WriteLine($"\n\nEncrypt ?");
        Console.ReadLine();
        Console.Clear();

        if (Key == null)
            Key = generateKey();

        Console.Clear();
        string result = Encrypt(userInput, Key);
        Console.Write($"\nencrypted string : ");
        Console.ForegroundColor = ConsoleColor.Blue;
        Console.Write($"{result}\n");
        Console.ForegroundColor = ConsoleColor.White;
        Console.Write($"\nYour key (save this!): ");
        Console.ForegroundColor = ConsoleColor.Red;
        Console.Write($"{Key}\n");
        Console.ForegroundColor = ConsoleColor.White;
        Console.ReadKey();
    }
    else
    {
        Console.Clear();
        if (Key == null)
        {
            Console.WriteLine("The encryption key");
            Console.Write("\n\n--> ");
            Key = Console.ReadLine();
        }

        Console.Clear();
        string result = Decrypt(userInput, Key);
        Console.WriteLine($"decrypted string : ");
        Console.ForegroundColor = ConsoleColor.Blue;
        Console.Write($"{result}");
        Console.ForegroundColor = ConsoleColor.White;
    }
}

bool SelectMode()
{
    string[] options = { "Encrypt", "Decrypt" };
    int selected = 0;

    ConsoleKey key;
    do
    {
        Console.Clear();
        Console.ForegroundColor = ConsoleColor.Red;
        Console.WriteLine("Cryptext");
        Console.ResetColor();
        Console.WriteLine("\nSelect mode:\n");

        for (int i = 0; i < options.Length; i++)
        {
            if (i == selected)
            {
                Console.ForegroundColor = ConsoleColor.Green;
                Console.WriteLine($"  > {options[i]}");
            }
            else
            {
                Console.ResetColor();
                Console.WriteLine($"    {options[i]}");
            }
        }

        Console.ResetColor();
        Console.WriteLine("\nUse arrow keys to navigate, Enter to select.");
        key = Console.ReadKey(true).Key;

        if (key == ConsoleKey.UpArrow)
            selected = (selected == 0) ? options.Length - 1 : selected - 1;
        else if (key == ConsoleKey.DownArrow)
            selected = (selected + 1) % options.Length;

    } while (key != ConsoleKey.Enter);

    return selected == 0;
}

string Encrypt(string plainText, string keyString)
{
    if (string.IsNullOrWhiteSpace(plainText))
        return "Error: The plaintext to encrypt cannot be empty.";

    byte[] plainTextBytes = Encoding.UTF8.GetBytes(plainText);

    if (string.IsNullOrWhiteSpace(keyString))
        return "Error: An encryption key is required.";

    byte[] finalKeyBytes = NormalizeKey(keyString);

    try
    {
        using (Aes aesAlg = Aes.Create())
        {
            aesAlg.Key = finalKeyBytes;
            aesAlg.GenerateIV();
            byte[] iv = aesAlg.IV;

            ICryptoTransform encryptor = aesAlg.CreateEncryptor(aesAlg.Key, iv);

            using (var msEncrypt = new MemoryStream())
            {
                msEncrypt.Write(iv, 0, iv.Length);

                using (var csEncrypt = new CryptoStream(msEncrypt, encryptor, CryptoStreamMode.Write))
                {
                    csEncrypt.Write(plainTextBytes, 0, plainTextBytes.Length);
                    csEncrypt.FlushFinalBlock();
                }

                return Convert.ToBase64String(msEncrypt.ToArray());
            }
        }
    }
    catch (Exception ex)
    {
        return $"Critical Encryption Error: {ex.Message}";
    }
}

string Decrypt(string cipherText, string keyString)
{
    if (string.IsNullOrWhiteSpace(cipherText))
        return "Error: The ciphertext to decrypt cannot be empty.";

    if (string.IsNullOrWhiteSpace(keyString))
        return "Error: A decryption key is required.";

    byte[] finalKeyBytes = NormalizeKey(keyString);
    byte[] cipherBytes;

    try
    {
        cipherBytes = Convert.FromBase64String(cipherText);
    }
    catch
    {
        return "Error: Ciphertext is not valid Base64.";
    }

    try
    {
        using (Aes aesAlg = Aes.Create())
        {
            aesAlg.Key = finalKeyBytes;

            byte[] iv = new byte[16];
            byte[] encryptedData = new byte[cipherBytes.Length - 16];

            Array.Copy(cipherBytes, 0, iv, 0, 16);
            Array.Copy(cipherBytes, 16, encryptedData, 0, encryptedData.Length);

            aesAlg.IV = iv;

            ICryptoTransform decryptor = aesAlg.CreateDecryptor(aesAlg.Key, aesAlg.IV);

            using (var msDecrypt = new MemoryStream(encryptedData))
            using (var csDecrypt = new CryptoStream(msDecrypt, decryptor, CryptoStreamMode.Read))
            using (var srDecrypt = new StreamReader(csDecrypt))
            {
                return srDecrypt.ReadToEnd();
            }
        }
    }
    catch (CryptographicException)
    {
        return "Error: Decryption failed. Check your key or ciphertext.";
    }
    catch (Exception ex)
    {
        return $"Critical Decryption Error: {ex.Message}";
    }
}

byte[] NormalizeKey(string keyString)
{
    byte[] initialKeyBytes = Encoding.UTF8.GetBytes(keyString);
    const int requiredKeySize = 32;
    byte[] finalKeyBytes = new byte[requiredKeySize];

    if (initialKeyBytes.Length > requiredKeySize)
        Array.Copy(initialKeyBytes, finalKeyBytes, Math.Min(initialKeyBytes.Length, requiredKeySize));
    else
    {
        Array.Copy(initialKeyBytes, finalKeyBytes, initialKeyBytes.Length);
        for (int i = initialKeyBytes.Length; i < requiredKeySize; i++)
            finalKeyBytes[i] = 0x00;
    }

    return finalKeyBytes;
}

string generateKey()
{
    byte[] keyBytes = new byte[32];

    using (var rng = RandomNumberGenerator.Create())
    {
        rng.GetBytes(keyBytes);
    }
    return Convert.ToBase64String(keyBytes);
}

void Wait()
{
    try { Console.ReadKey(true); } catch (InvalidOperationException) { }
}
