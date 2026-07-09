const path = require('path');
const Database = require('better-sqlite3');

const DB_PATH = path.join(__dirname, '..', 'LabManager.db');
const DATA_PATH = path.join(__dirname, 'data.json');

const db = new Database(DB_PATH, { readonly: true });
const experiments = db.prepare('SELECT * FROM Experiments ORDER BY LastModified DESC').all();
db.close();

const byStatus = {};
const languages = {};

for (const e of experiments) {
    byStatus[e.Status] = (byStatus[e.Status] || 0) + 1;
    if (e.Language) languages[e.Language] = (languages[e.Language] || 0) + 1;
}

const stats = {
    total: experiments.length,
    byStatus: Object.entries(byStatus).map(([Status, count]) => ({ Status, count })),
    languages: Object.entries(languages).map(([Language, count]) => ({ Language, count }))
};

const fs = require('fs');
fs.writeFileSync(DATA_PATH, JSON.stringify({ experiments, stats }, null, 2));
console.log(`Exported ${experiments.length} experiments to data.json`);
