import React from 'react'

const s = {
  root: { display: 'flex', flexDirection: 'column', height: '100%' },
  header: {
    display: 'flex', alignItems: 'center', justifyContent: 'space-between',
    padding: '14px 16px',
    borderBottom: '1px solid #2d2d4a',
    fontSize: 13, fontWeight: 600, color: '#94a3b8',
    textTransform: 'uppercase', letterSpacing: 1,
  },
  refreshBtn: {
    background: 'none', border: 'none', color: '#4a4a6a',
    cursor: 'pointer', fontSize: 16, padding: 0,
    lineHeight: 1,
  },
  list: { flex: 1, overflowY: 'auto', padding: '8px 0' },
  empty: { padding: 20, fontSize: 13, color: '#4a4a6a', textAlign: 'center' },
  item: {
    padding: '10px 16px',
    borderBottom: '1px solid #1e1e30',
    display: 'flex',
    alignItems: 'flex-start',
    gap: 10,
    cursor: 'pointer',
  },
  itemInfo: { flex: 1, minWidth: 0 },
  itemTitle: { fontSize: 13, color: '#e2e8f0', fontWeight: 500, overflow: 'hidden', textOverflow: 'ellipsis', whiteSpace: 'nowrap' },
  itemMeta: { fontSize: 11, color: '#4a4a6a', marginTop: 2 },
  deleteBtn: {
    background: 'none', border: 'none', color: '#4a4a6a',
    cursor: 'pointer', fontSize: 14, padding: '2px 4px',
    borderRadius: 4, flexShrink: 0,
    transition: 'color 0.15s',
  },
}

export default function SceneList({ scenes, onLoad, onDelete, onRefresh }) {
  return (
    <div style={s.root}>
      <div style={s.header}>
        <span>Saved Scenes</span>
        <button style={s.refreshBtn} onClick={onRefresh} title="Refresh list">↻</button>
      </div>
      <div style={s.list}>
        {scenes.length === 0 && (
          <p style={s.empty}>No saved scenes yet.<br />Generate one and hit Save!</p>
        )}
        {scenes.map(sc => (
          <div
            key={sc.id}
            style={s.item}
            onMouseEnter={e => e.currentTarget.style.background = '#1a1a2e'}
            onMouseLeave={e => e.currentTarget.style.background = ''}
            onClick={() => onLoad(sc.file)}
          >
            <div style={s.itemInfo}>
              <div style={s.itemTitle}>{sc.title || sc.id}</div>
              <div style={s.itemMeta}>
                {[sc.subject && sc.subject.replace(/_/g,' '), sc.grade && `Grade ${sc.grade}`]
                  .filter(Boolean).join(' · ')}
              </div>
            </div>
            <button
              style={s.deleteBtn}
              title="Delete"
              onClick={e => { e.stopPropagation(); onDelete(sc.id) }}
              onMouseEnter={e => e.currentTarget.style.color = '#f87171'}
              onMouseLeave={e => e.currentTarget.style.color = '#4a4a6a'}
            >
              ✕
            </button>
          </div>
        ))}
      </div>
    </div>
  )
}
