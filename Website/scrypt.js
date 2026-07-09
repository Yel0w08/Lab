let experiments = [];
let filtered = [];
let filter = 'all';
let query = '';
let sortKey = null;
let sortDir = null;

const $ = s => document.querySelector(s);
const $$ = s => document.querySelectorAll(s);

const tbody = $('#tbody');
const search = $('#search');
const loader = $('#loader');
const hStats = $('#hStats');
const modal = $('#modal');
const mTitle = $('#mTitle');
const mBody = $('#mBody');
const mClose = $('#mClose');
const mBackdrop = $('#mBackdrop');

const statusLabel = s => ({ InProgress: 'active' })[s] || s.toLowerCase();
const fmtDate = d => d ? new Date(d).toISOString().slice(0, 10) : '--';

async function load() {
    loader.classList.add('show');
    try {
        const params = new URLSearchParams();
        if (query) params.set('search', query);
        if (filter !== 'all') params.set('status', filter);
        const qs = params.toString();
        const url = '/api/experiments' + (qs ? '?' + qs : '');
        const res = await fetch(url);
        if (!res.ok) throw Error();
        experiments = filtered = await res.json();
        sort();
        render();
    } catch (_) {
        try {
            const r = await fetch('data.json');
            const d = await r.json();
            experiments = d.experiments;
            applyClient();
            render();
        } catch (_2) {
            tbody.innerHTML = '<tr class="empty"><td colspan="6">failed to load experiments</td></tr>';
        }
    }
    loader.classList.remove('show');
    fetchStats();
}

async function fetchStats() {
    try {
        const res = await fetch('/api/stats');
        if (!res.ok) throw Error();
        const s = await res.json();
        const labels = { Finished: 'done', InProgress: 'active', Planned: 'planned', Archived: 'archived', Abandoned: 'dead' };
        let html = `<span><span class="n">${s.total}</span> total</span>`;
        for (const st of s.byStatus) {
            html += `<span><span class="n">${st.count}</span> ${labels[st.Status] || st.Status.toLowerCase()}</span>`;
        }
        hStats.innerHTML = html;
    } catch (_) {}
}

function applyClient() {
    let r = [...experiments];
    if (filter !== 'all') r = r.filter(e => e.Status === filter);
    if (query) {
        const q = query.toLowerCase();
        r = r.filter(e =>
            (e.Name||'').toLowerCase().includes(q) ||
            (e.Description||'').toLowerCase().includes(q) ||
            (e.Language||'').toLowerCase().includes(q) ||
            (e.Framework||'').toLowerCase().includes(q) ||
            (e.Tags||'').toLowerCase().includes(q)
        );
    }
    filtered = r;
}

function sort() {
    if (!sortKey) return;
    filtered.sort((a, b) => {
        let va = (a[sortKey] || '').toString().toLowerCase();
        let vb = (b[sortKey] || '').toString().toLowerCase();
        if (sortKey === 'LastModified') {
            va = a[sortKey] ? new Date(a[sortKey]).getTime() : 0;
            vb = b[sortKey] ? new Date(b[sortKey]).getTime() : 0;
        }
        if (va < vb) return sortDir === 'asc' ? -1 : 1;
        if (va > vb) return sortDir === 'asc' ? 1 : -1;
        return 0;
    });
}

function render() {
    if (!filtered.length) {
        tbody.innerHTML = '<tr class="empty"><td colspan="6">no experiments found</td></tr>';
        return;
    }

    tbody.innerHTML = filtered.map(e => {
        const fav = e.Favorite ? '<span class="fav">&#9733;</span>' : '';
        const desc = e.Description || '';
        const tags = e.Tags ? e.Tags.split(',').filter(Boolean).map(t =>
            `<span class="tg-item">${t.trim()}</span>`
        ).join('') : '';

        return `<tr data-id="${e.Id}">
            <td><div class="e-name">${e.Name}${fav}</div>${desc ? `<div class="e-desc">${desc}</div>` : ''}</td>
            <td><span class="st st-${e.Status}">${statusLabel(e.Status)}</span></td>
            <td>${e.Language ? `<span class="lt">${e.Language}</span>` : '<span class="cd">--</span>'}</td>
            <td><span class="cf">${e.Framework || '--'}</span></td>
            <td>${tags ? `<div class="tg">${tags}</div>` : '<span class="cd">--</span>'}</td>
            <td><span class="cd">${fmtDate(e.LastModified)}</span></td>
        </tr>`;
    }).join('');

    tbody.querySelectorAll('tr').forEach(el => {
        el.addEventListener('click', () => {
            const id = parseInt(el.dataset.id);
            const exp = experiments.find(e => e.Id === id);
            if (exp) openModal(exp);
        });
    });
}

function openModal(e) {
    mTitle.textContent = e.Name + (e.Favorite ? ' \u2605' : '');

    const tags = e.Tags ? e.Tags.split(',').filter(Boolean).map(t =>
        `<span class="tg-item">${t.trim()}</span>`
    ).join('') : '';

    const details = [];
    if (e.Language) details.push(`<div class="db"><div class="dl">Language</div><div class="dv">${e.Language}</div></div>`);
    if (e.Framework) details.push(`<div class="db"><div class="dl">Framework</div><div class="dv">${e.Framework}</div></div>`);
    if (e.Engine) details.push(`<div class="db"><div class="dl">Engine</div><div class="dv">${e.Engine}</div></div>`);
    if (e.CreatedAt) details.push(`<div class="db"><div class="dl">Created</div><div class="dv">${fmtDate(e.CreatedAt)}</div></div>`);
    if (e.LastModified) details.push(`<div class="db"><div class="dl">Modified</div><div class="dv">${fmtDate(e.LastModified)}</div></div>`);

    mBody.innerHTML = `
        <div class="mf">
            <div class="lbl">Status</div>
            <div class="val"><span class="st st-${e.Status}">${statusLabel(e.Status)}</span></div>
        </div>
        <div class="mf">
            <div class="lbl">Description</div>
            <div class="val">${e.Description || '<span style="color:var(--txt3)">no description</span>'}</div>
        </div>
        ${details.length ? `<div class="mf"><div class="lbl">Details</div><div class="m-details">${details.join('')}</div></div>` : ''}
        ${tags ? `<div class="mf"><div class="lbl">Tags</div><div class="tg">${tags}</div></div>` : ''}
        <div class="mf">
            <div class="lbl">Notes</div>
            <div class="m-notes">${e.Notes || '<span style="color:var(--txt3)">no notes</span>'}</div>
        </div>
    `;

    modal.classList.add('open');
}

function closeModal() { modal.classList.remove('open'); }

mClose.addEventListener('click', closeModal);
mBackdrop.addEventListener('click', closeModal);
document.addEventListener('keydown', e => { if (e.key === 'Escape') closeModal(); });

// Filter buttons
$$('.filter-btn').forEach(btn => {
    btn.addEventListener('click', () => {
        $$('.filter-btn').forEach(b => b.classList.remove('active'));
        btn.classList.add('active');
        filter = btn.dataset.f === 'all' ? 'all' : btn.dataset.f;
        query = search.value;
        load();
    });
});

// Search with debounce
let st;
search.addEventListener('input', () => {
    clearTimeout(st);
    st = setTimeout(() => {
        query = search.value;
        filter = $$('.filter-btn.active')[0]?.dataset.f || 'all';
        load();
    }, 250);
});

// Column sorting
$$('th[data-sort]').forEach(th => {
    th.addEventListener('click', () => {
        const key = th.dataset.sort;
        $$('th[data-sort]').forEach(t => {
            t.classList.remove('asc', 'desc');
        });
        if (sortKey === key) {
            if (sortDir === 'asc') { sortDir = 'desc'; th.classList.add('desc'); }
            else { sortDir = 'asc'; th.classList.add('asc'); }
        } else {
            sortKey = key;
            sortDir = 'asc';
            th.classList.add('asc');
        }
        sort();
        render();
    });
});

load();
