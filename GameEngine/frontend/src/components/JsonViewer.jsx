import React, { useMemo, useState } from 'react'

const s = {
  root: { display: 'flex', flexDirection: 'column', height: '100%', overflow: 'hidden' },
  header: {
    padding: '14px 16px',
    borderBottom: '1px solid #2d2d4a',
    fontSize: 13, fontWeight: 600, color: '#94a3b8',
    textTransform: 'uppercase', letterSpacing: 1,
    display: 'flex', alignItems: 'center', justifyContent: 'space-between',
  },
  copyBtn: {
    padding: '4px 10px', borderRadius: 6,
    background: '#2d2d4a', border: 'none',
    color: '#94a3b8', fontSize: 12, cursor: 'pointer',
  },
  pre: {
    flex: 1, overflow: 'auto', margin: 0, padding: 16,
    fontFamily: '"Fira Mono", "Cascadia Code", monospace',
    fontSize: 12, lineHeight: 1.6,
    color: '#cbd5e1',
    whiteSpace: 'pre-wrap', wordBreak: 'break-word',
  },
  empty: {
    flex: 1, display: 'flex', alignItems: 'center', justifyContent: 'center',
    flexDirection: 'column', gap: 12, color: '#4a4a6a', fontSize: 13,
    textAlign: 'center', padding: 24,
  },
  summaryBar: {
    padding: '8px 16px',
    borderTop: '1px solid #2d2d4a',
    fontSize: 12, color: '#4a4a6a',
    display: 'flex', gap: 16,
  },
  stat: { display: 'flex', gap: 4 },
  statVal: { color: '#7c6fff', fontWeight: 600 },
}

function colorize(json) {
  return json
    .replace(/("(\\u[a-zA-Z0-9]{4}|\\[^u]|[^\\"])*"(\s*:)?)/g, (match) => {
      const isKey = /:$/.test(match)
      const color = isKey ? '#7c6fff' : '#86efac'
      return `<span style="color:${color}">${match}</span>`
    })
    .replace(/\b(true|false)\b/g, '<span style="color:#f472b6">$1</span>')
    .replace(/\b(null)\b/g, '<span style="color:#94a3b8">$1</span>')
    .replace(/\b(-?\d+\.?\d*)\b/g, '<span style="color:#fb923c">$1</span>')
}

function SceneSummary({ scene }) {
  const entities = scene?.entities || []
  const counts = entities.reduce((acc, e) => {
    const tag = e.tags?.[0] || 'other'
    acc[tag] = (acc[tag] || 0) + 1
    return acc
  }, {})
  return (
    <div style={s.summaryBar}>
      {Object.entries(counts).map(([tag, n]) => (
        <span key={tag} style={s.stat}>
          <span style={s.statVal}>{n}</span>
          <span>{tag}{n > 1 && tag !== 'enemy' ? 's' : ''}</span>
        </span>
      ))}
    </div>
  )
}

export default function JsonViewer({ scene }) {
  const [copied, setCopied] = useState(false)

  const formatted = useMemo(() =>
    scene ? JSON.stringify(scene, null, 2) : '',
  [scene])

  const colorized = useMemo(() =>
    formatted ? colorize(formatted.replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;')) : '',
  [formatted])

  const handleCopy = () => {
    navigator.clipboard.writeText(formatted)
    setCopied(true)
    setTimeout(() => setCopied(false), 1500)
  }

  return (
    <div style={s.root}>
      <div style={s.header}>
        <span>Scene JSON</span>
        {scene && (
          <button style={s.copyBtn} onClick={handleCopy}>
            {copied ? '✓ Copied' : '⎘ Copy'}
          </button>
        )}
      </div>

      {scene ? (
        <>
          <pre
            style={s.pre}
            dangerouslySetInnerHTML={{ __html: colorized }}
          />
          <SceneSummary scene={scene} />
        </>
      ) : (
        <div style={s.empty}>
          <span style={{ fontSize: 40 }}>🎮</span>
          <span>Your generated scene JSON<br />will appear here.</span>
          <span style={{ fontSize: 11, color: '#2d2d4a' }}>
            Type a prompt and click Generate Game
          </span>
        </div>
      )}
    </div>
  )
}
