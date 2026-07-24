namespace Manager.Data;

public static class DatabaseConfig
{
    public static string ConnectionString { get; } =
        $"Data Source={DbPath}";

    private static string DbPath =>
        Path.Combine(RepoRoot, "LabManager.db");

    private static string RepoRoot
    {
        get
        {
            var dir = new DirectoryInfo(
                Path.GetFullPath(Path.Combine(
                    AppDomain.CurrentDomain.BaseDirectory,
                    "..", "..", ".."
                ))
            );

            while (dir is not null)
            {
                if (Directory.Exists(Path.Combine(dir.FullName, ".git")))
                    return dir.FullName;
                dir = dir.Parent;
            }

            return AppDomain.CurrentDomain.BaseDirectory;
        }
    }
}
