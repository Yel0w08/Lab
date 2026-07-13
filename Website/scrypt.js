let experiments = [];
let filtered = [];
let filter = 'all';
let query = '';
let activeTags = [];

const $ = s => document.querySelector(s);
const $$ = s => document.querySelectorAll(s);

const tbody = $('#tbody');
const search = $('#search');
const clearBtn = $('#clearSearch');
const loader = $('#loader');
const hStats = $('#hStats');
const tagBar = $('#tagBar');
const filterRow = $('#filterRow');
const modal = $('#modal');
const mTitle = $('#mTitle');
const mBody = $('#mBody');
const mX = $('#mX');
const mShade = $('#mShade');

const label = s => ({ InProgress: 'in progress' })[s] || s.toLowerCase();
const fmt = d => d ? new Date(d).toISOString().slice(0, 10) : '--';

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
        experiments = await res.json();
        applyClient();
        render();
    } catch (_) {
        try {
            const r = await fetch('data.json');
            const d = await r.json();
            experiments = d.experiments;
            applyClient();
            render();
        } catch (_2) {
            tbody.innerHTML = '<tr class="empty"><td colspan="6">failed to connect</td></tr>';
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
        let html = `<div class="hs-item"><span class="hs-n">${s.total}</span><span class="hs-l">total</span></div>`;
        for (const st of s.byStatus) {
            html += `<div class="hs-item"><span class="hs-n">${st.count}</span><span class="hs-l">${labels[st.Status] || st.Status.toLowerCase()}</span></div>`;
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
    if (activeTags.length) {
        r = r.filter(e => {
            const tags = (e.Tags||'').split(',').map(t => t.trim().toLowerCase());
            return activeTags.every(t => tags.includes(t.toLowerCase()));
        });
    }
    filtered = r;
    renderTagBar();
}

function renderTagBar() {
    if (!activeTags.length) { tagBar.innerHTML = ''; return; }
    tagBar.innerHTML = activeTags.map(t =>
        `<span class="tg active" data-tag="${t}">#${t}<span class="tg-x" data-rm="${t}">&times;</span></span>`
    ).join('') +
    '<span class="tg-clr" id="clearTags">clear all</span>';

    tagBar.querySelectorAll('[data-rm]').forEach(el => {
        el.addEventListener('click', e => {
            e.stopPropagation();
            const tag = el.dataset.rm;
            activeTags = activeTags.filter(t => t !== tag);
            load();
        });
    });

    const clearTags = document.getElementById('clearTags');
    if (clearTags) {
        clearTags.addEventListener('click', () => {
            activeTags = [];
            load();
        });
    }
}

function render() {
    if (!filtered.length) {
        tbody.innerHTML = '<tr class="empty"><td colspan="6">no experiments match your criteria</td></tr>';
        return;
    }

    tbody.innerHTML = filtered.map(e => {
        const fav = e.Favorite ? '<span class="fav">&#9733;</span>' : '';
        const desc = e.Description || '';
        const tags = e.Tags ? e.Tags.split(',').filter(Boolean).map(t =>
            `<span class="tg-item" data-tag="${t.trim()}">${t.trim()}</span>`
        ).join('') : '';

        return `<tr data-id="${e.Id}">
            <td><div class="e-name">${e.Name}${fav}</div>${desc ? `<div class="e-desc">${desc}</div>` : ''}</td>
            <td><span class="st st-${e.Status}">${label(e.Status)}</span></td>
            <td>${e.Language ? `<span class="lt">${e.Language}</span>` : '<span class="cd">&mdash;</span>'}</td>
            <td><span class="cf">${e.Framework || '&mdash;'}</span></td>
            <td>${tags ? `<div class="tg-r">${tags}</div>` : '<span class="cd">&mdash;</span>'}</td>
            <td><span class="cd">${fmt(e.LastModified)}</span></td>
        </tr>`;
    }).join('');

    // row click -> modal
    tbody.querySelectorAll('tr').forEach(el => {
        el.addEventListener('click', e => {
            if (e.target.closest('.tg-item')) return;
            const id = parseInt(el.dataset.id);
            const exp = experiments.find(e => e.Id === id);
            if (exp) openModal(exp);
        });
    });

    // clickable tags
    tbody.querySelectorAll('.tg-item').forEach(el => {
        el.addEventListener('click', e => {
            e.stopPropagation();
            const tag = el.dataset.tag;
            if (!activeTags.includes(tag)) {
                activeTags.push(tag);
                load();
            }
        });
    });
}

function openModal(e) {
    mTitle.textContent = e.Name + (e.Favorite ? ' \u2605' : '');

    const tags = e.Tags ? e.Tags.split(',').filter(Boolean).map(t =>
        `<span class="tg-item" style="cursor:default">${t.trim()}</span>`
    ).join('') : '';

    const details = [];
    if (e.Language) details.push(`<div class="db"><div class="dl">Language</div><div class="dv">${e.Language}</div></div>`);
    if (e.Framework) details.push(`<div class="db"><div class="dl">Framework</div><div class="dv">${e.Framework}</div></div>`);
    if (e.Engine) details.push(`<div class="db"><div class="dl">Engine</div><div class="dv">${e.Engine}</div></div>`);
    if (e.CreatedAt) details.push(`<div class="db"><div class="dl">Created</div><div class="dv">${fmt(e.CreatedAt)}</div></div>`);
    if (e.LastModified) details.push(`<div class="db"><div class="dl">Updated</div><div class="dv">${fmt(e.LastModified)}</div></div>`);

    mBody.innerHTML = `
        <div class="mf">
            <div class="lbl">Status</div>
            <div class="val"><span class="st st-${e.Status}">${label(e.Status)}</span></div>
        </div>
        <div class="mf">
            <div class="lbl">Description</div>
            <div class="val">${e.Description || '<span style="color:var(--txt4)">no description</span>'}</div>
        </div>
        ${details.length ? `<div class="mf"><div class="lbl">Details</div><div class="m-dt">${details.join('')}</div></div>` : ''}
        ${tags ? `<div class="mf"><div class="lbl">Tags</div><div class="tg-r">${tags}</div></div>` : ''}
        <div class="mf">
            <div class="lbl">Notes</div>
            <div class="m-nt">${e.Notes || '<span style="color:var(--txt4)">no notes</span>'}</div>
        </div>
    `;

    modal.classList.add('open');
}

function closeModal() { modal.classList.remove('open'); }
mX.addEventListener('click', closeModal);
mShade.addEventListener('click', closeModal);
document.addEventListener('keydown', e => { if (e.key === 'Escape') closeModal(); });

// Filter buttons
filterRow.querySelectorAll('.flt').forEach(btn => {
    btn.addEventListener('click', () => {
        filterRow.querySelectorAll('.flt').forEach(b => b.classList.remove('active'));
        btn.classList.add('active');
        filter = btn.dataset.f === 'all' ? 'all' : btn.dataset.f;
        load();
    });
});

// Search
let st;
search.addEventListener('input', () => {
    clearBtn.classList.toggle('show', search.value.length > 0);
    clearTimeout(st);
    st = setTimeout(() => {
        query = search.value;
        load();
    }, 250);
});

clearBtn.addEventListener('click', () => {
    search.value = '';
    clearBtn.classList.remove('show');
    query = '';
    load();
});

load();
