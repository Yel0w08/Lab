import sqlite3, json, os

db_path = os.path.join(os.path.dirname(__file__), '..', 'LabManager.db')
out_path = os.path.join(os.path.dirname(__file__), 'data.json')

conn = sqlite3.connect(db_path)
conn.row_factory = sqlite3.Row
rows = conn.execute('SELECT * FROM Experiments ORDER BY LastModified DESC').fetchall()
conn.close()

experiments = [dict(r) for r in rows]

by_status = {}
languages = {}
for e in experiments:
    s = e['Status']
    by_status[s] = by_status.get(s, 0) + 1
    l = e.get('Language')
    if l:
        languages[l] = languages.get(l, 0) + 1

stats = {
    'total': len(experiments),
    'byStatus': [{'Status': k, 'count': v} for k, v in by_status.items()],
    'languages': [{'Language': k, 'count': v} for k, v in languages.items()]
}

with open(out_path, 'w') as f:
    json.dump({'experiments': experiments, 'stats': stats}, f, indent=2)

print(f'Exported {len(experiments)} experiments to data.json')
