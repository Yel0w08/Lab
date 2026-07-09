const path = require('path');
const fs = require('fs');
const Database = require('better-sqlite3');

const DB_PATH = path.join(__dirname, '..', 'LabManager.db');
const OUT_PATH = path.join(__dirname, 'data.json');

const db = new Database(DB_PATH, { readonly: true });
const experiments = db.prepare('SELECT * FROM Experiments ORDER BY LastModified DESC').all();

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

fs.writeFileSync(OUT_PATH, JSON.stringify({ experiments, stats }, null, 2));
console.log(`Exported ${experiments.length} experiments to data.json`);
