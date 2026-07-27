# LabManager Database Schema

Database file: `LabManager.db` (SQLite, located at repo root)

## Experiments Table

| Field         | Type      | Description                                    |
|---------------|-----------|------------------------------------------------|
| Id            | INTEGER   | Primary key, auto-increment                   |
| Name          | TEXT(200) | Experiment name (required)                     |
| Description   | TEXT(2000)| Short description of the experiment            |
| Language      | TEXT(100) | Programming language used                      |
| Framework     | TEXT(100) | Framework or library used                      |
| Engine        | TEXT(100) | Runtime or engine used                         |
| Status        | TEXT(50)  | One of: Planned, InProgress, Finished, Archived, Abandoned |
| ProjectPath   | TEXT(500) | Local filesystem path to the project           |
| Tags          | TEXT(1000)| Comma-separated list of tags                   |
| Favorite      | BOOLEAN   | Whether the experiment is marked as favorite   |
| Downloadable  | BOOLEAN   | Whether the experiment can be downloaded       |
| Notes         | TEXT(4000)| Freeform notes                                |
| CreatedAt     | DATETIME  | Timestamp when the experiment was created      |
| LastModified  | DATETIME  | Timestamp when the experiment was last updated |
