let experiments = [];
let filtered = [];
let currentFilter = 'all';
let currentSearch = '';

const tbody = document.getElementById('tbody');
const searchInput = document.getElementById('search');
const filterBtns = document.querySelectorAll('.filter');
const loading = document.getElementById('loading');
const navStats = document.getElementById('navStats');
const modal = document.getElementById('modal');
const modalTitle = document.getElementById('modalTitle');
const modalBody = document.getElementById('modalBody');
const modalClose = document.getElementById('modalClose');

function statusLabel(s) {
    const map = { InProgress: 'in progress' };
    return map[s] || s.toLowerCase();
}

function fmtDate(d) {
    if (!d) return '--';
    const dt = new Date(d);
    const y = dt.getFullYear();
    const m = String(dt.getMonth() + 1).padStart(2, '0');
    const day = String(dt.getDate()).padStart(2, '0');
    return `${y}-${m}-${day}`;
}

async function load() {
    loading.classList.add('show');
    try {
        const r = await fetch('data.json');
        if (!r.ok) throw new Error();
        const d = await r.json();
        experiments = d.experiments;
        apply();
        renderStats(d.stats);
    } catch (_) {
        tbody.innerHTML = '<tr class="empty"><td colspan="6">failed to load experiments</td></tr>';
    }
    loading.classList.remove('show');
}

function renderStats(s) {
    const labels = { Finished: 'done', InProgress: 'active', Planned: 'planned', Archived: 'archived', Abandoned: 'dead' };
    let html = `<span><span class="num">${s.total}</span> total</span>`;
    for (const st of s.byStatus) {
        html += `<span><span class="num">${st.count}</span> ${labels[st.Status] || st.Status.toLowerCase()}</span>`;
    }
    navStats.innerHTML = html;
}

function apply() {
    let result = [...experiments];

    if (currentFilter !== 'all') {
        result = result.filter(e => e.Status === currentFilter);
    }

    if (currentSearch) {
        const q = currentSearch.toLowerCase();
        result = result.filter(e =>
            (e.Name || '').toLowerCase().includes(q) ||
            (e.Description || '').toLowerCase().includes(q) ||
            (e.Language || '').toLowerCase().includes(q) ||
            (e.Framework || '').toLowerCase().includes(q) ||
            (e.Tags || '').toLowerCase().includes(q) ||
            (e.Notes || '').toLowerCase().includes(q)
        );
    }

    filtered = result;
    render();
}

function render() {
    if (filtered.length === 0) {
        tbody.innerHTML = '<tr class="empty"><td colspan="6">no matching experiments</td></tr>';
        return;
    }

    tbody.innerHTML = filtered.map(e => {
        const fav = e.Favorite ? '<span class="fav">&#9733;</span>' : '';
        const desc = e.Description || '';
        const tags = e.Tags ? e.Tags.split(',').filter(Boolean).map(t =>
            `<span class="tag-item">${t.trim()}</span>`
        ).join('') : '';

        return `<tr data-id="${e.Id}">
            <td>
                <div class="experiment-name">${e.Name}${fav}</div>
                ${desc ? `<div class="experiment-desc">${desc}</div>` : ''}
            </td>
            <td><span class="status status-${e.Status}">${statusLabel(e.Status)}</span></td>
            <td>${e.Language ? `<span class="lang-tag">${e.Language}</span>` : '<span class="cell-date">--</span>'}</td>
            <td><span class="cell-framework">${e.Framework || '--'}</span></td>
            <td>${tags ? `<div class="tag-list">${tags}</div>` : '<span class="cell-date">--</span>'}</td>
            <td><span class="cell-date">${fmtDate(e.LastModified)}</span></td>
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
    modalTitle.textContent = e.Name + (e.Favorite ? ' \u2605' : '');

    const tags = e.Tags ? e.Tags.split(',').filter(Boolean).map(t =>
        `<span class="tag-item">${t.trim()}</span>`
    ).join('') : '';

    const details = [];
    if (e.Language) details.push(`<div class="detail-box"><div class="lbl">Language</div><div class="val">${e.Language}</div></div>`);
    if (e.Framework) details.push(`<div class="detail-box"><div class="lbl">Framework</div><div class="val">${e.Framework}</div></div>`);
    if (e.Engine) details.push(`<div class="detail-box"><div class="lbl">Engine</div><div class="val">${e.Engine}</div></div>`);
    if (e.CreatedAt) details.push(`<div class="detail-box"><div class="lbl">Created</div><div class="val">${fmtDate(e.CreatedAt)}</div></div>`);
    if (e.LastModified) details.push(`<div class="detail-box"><div class="lbl">Modified</div><div class="val">${fmtDate(e.LastModified)}</div></div>`);

    modalBody.innerHTML = `
        <div class="modal-field">
            <div class="label">Status</div>
            <div class="value"><span class="status status-${e.Status}">${statusLabel(e.Status)}</span></div>
        </div>
        <div class="modal-field">
            <div class="label">Description</div>
            <div class="value">${e.Description || 'No description.'}</div>
        </div>
        ${details.length ? `<div class="modal-field"><div class="label">Details</div><div class="modal-details">${details.join('')}</div></div>` : ''}
        ${tags ? `<div class="modal-field"><div class="label">Tags</div><div class="tag-list">${tags}</div></div>` : ''}
        <div class="modal-field">
            <div class="label">Notes</div>
            <div class="modal-notes">${e.Notes || 'No notes.'}</div>
        </div>
    `;

    modal.classList.add('open');
}

function closeModal() {
    modal.classList.remove('open');
}

modalClose.addEventListener('click', closeModal);
modal.addEventListener('click', e => { if (e.target === modal) closeModal(); });
document.addEventListener('keydown', e => { if (e.key === 'Escape') closeModal(); });

searchInput.addEventListener('input', e => {
    currentSearch = e.target.value;
    apply();
});

filterBtns.forEach(btn => {
    btn.addEventListener('click', () => {
        filterBtns.forEach(b => b.classList.remove('active'));
        btn.classList.add('active');
        currentFilter = btn.dataset.filter;
        apply();
    });
});

load();
