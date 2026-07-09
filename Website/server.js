const express = require('express');
const cors = require('cors');
const path = require('path');
const Database = require('better-sqlite3');

const app = express();
const PORT = process.env.PORT || 3000;
const DB_PATH = path.join(__dirname, '..', 'LabManager.db');

app.use(cors());
app.use(express.json());
app.use(express.static(path.join(__dirname)));

function getDb() {
    const db = new Database(DB_PATH, { readonly: true });
    db.pragma('journal_mode = WAL');
    return db;
}

app.get('/api/experiments', (req, res) => {
    try {
        const db = getDb();
        const { search, status } = req.query;

        let sql = 'SELECT * FROM Experiments WHERE 1=1';
        const params = [];

        if (search) {
            sql += ' AND (Name LIKE ? OR Description LIKE ? OR Language LIKE ? OR Framework LIKE ? OR Tags LIKE ? OR Notes LIKE ?)';
            const q = `%${search}%`;
            params.push(q, q, q, q, q, q);
        }

        if (status) {
            sql += ' AND Status = ?';
            params.push(status);
        }

        sql += ' ORDER BY LastModified DESC';

        const rows = db.prepare(sql).all(...params);
        db.close();
        res.json(rows);
    } catch (err) {
        console.error(err);
        res.status(500).json({ error: err.message });
    }
});

app.get('/api/experiments/:id', (req, res) => {
    try {
        const db = getDb();
        const row = db.prepare('SELECT * FROM Experiments WHERE Id = ?').get(req.params.id);
        db.close();
        if (!row) return res.status(404).json({ error: 'Not found' });
        res.json(row);
    } catch (err) {
        res.status(500).json({ error: err.message });
    }
});

app.get('/api/stats', (req, res) => {
    try {
        const db = getDb();
        const total = db.prepare('SELECT COUNT(*) as count FROM Experiments').get();
        const byStatus = db.prepare('SELECT Status, COUNT(*) as count FROM Experiments GROUP BY Status').all();
        const languages = db.prepare('SELECT Language, COUNT(*) as count FROM Experiments GROUP BY Language').all();
        db.close();
        res.json({ total: total.count, byStatus, languages });
    } catch (err) {
        res.status(500).json({ error: err.message });
    }
});

app.listen(PORT, () => {
    console.log(`Yel0w's Lab running at http://localhost:${PORT}`);
});
