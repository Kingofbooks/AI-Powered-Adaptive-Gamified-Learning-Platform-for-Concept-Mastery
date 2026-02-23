import React, { useState, useEffect } from 'react'

const SUBJECTS = [
  '', 'photosynthesis', 'space', 'water_cycle', 'mathematics',
  'history', 'chemistry', 'physics', 'biology', 'geography',
]

const s = {
  panel: { display: 'flex', flexDirection: 'column', gap: 20, maxWidth: 640 },
  card: {
    background: '#1a1a2e',
    border: '1px solid #2d2d4a',
    borderRadius: 12,
    padding: 20,
  },
  cardTitle: { fontSize: 13, fontWeight: 600, color: '#94a3b8', marginBottom: 14, textTransform: 'uppercase', letterSpacing: 1 },
  label: { display: 'block', fontSize: 13, color: '#94a3b8', marginBottom: 6 },
  textarea: {
    width: '100%', minHeight: 110, padding: '10px 14px',
    background: '#0f0f1a', border: '1px solid #3d3d5a',
    borderRadius: 8, color: '#e2e8f0', fontSize: 14,
    resize: 'vertical', fontFamily: 'inherit',
    outline: 'none',
  },
  row: { display: 'flex', gap: 16 },
  select: {
    flex: 1, padding: '10px 14px',
    background: '#0f0f1a', border: '1px solid #3d3d5a',
    borderRadius: 8, color: '#e2e8f0', fontSize: 14,
    cursor: 'pointer', outline: 'none',
  },
  gradeBox: { flex: 1 },
  gradeVal: { display: 'flex', justifyContent: 'space-between', marginBottom: 4 },
  slider: { width: '100%', accentColor: '#7c6fff' },
  actions: { display: 'flex', gap: 10 },
  btn: (primary, disabled) => ({
    flex: primary ? 1 : 'none',
    padding: '11px 20px',
    borderRadius: 8,
    border: 'none',
    cursor: disabled ? 'not-allowed' : 'pointer',
    fontWeight: 600,
    fontSize: 14,
    background: disabled
      ? '#2d2d4a'
      : primary ? '#7c6fff' : '#2d2d4a',
    color: disabled ? '#4a4a6a' : '#fff',
    transition: 'opacity 0.15s',
    opacity: disabled ? 0.6 : 1,
  }),
  tip: {
    background: '#1a1a2e',
    border: '1px solid #2d2d4a',
    borderRadius: 12,
    padding: 16,
    fontSize: 13,
    color: '#64748b',
    lineHeight: 1.7,
  },
}

export default function GeneratorPanel({ onGenerate, onSave, onDownload, loading, hasScene }) {
  const [prompt,  setPrompt]  = useState('')
  const [subject, setSubject] = useState('')
  const [grade,   setGrade]   = useState(5)

  const canGenerate = prompt.trim().length > 3 && !loading

  const handleSubmit = (e) => {
    e.preventDefault()
    if (!canGenerate) return
    onGenerate({ prompt: prompt.trim(), subject, grade })
  }

  return (
    <div style={s.panel}>
      {/* Generator form */}
      <div style={s.card}>
        <p style={s.cardTitle}>Generate Game Scene</p>
        <form onSubmit={handleSubmit} style={{ display: 'flex', flexDirection: 'column', gap: 16 }}>
          <div>
            <label style={s.label}>Describe the game you want to create</label>
            <textarea
              style={s.textarea}
              placeholder="e.g. &quot;A platformer where students collect CO₂, H₂O and sunlight to make glucose — teach photosynthesis for grade 5&quot;"
              value={prompt}
              onChange={e => setPrompt(e.target.value)}
              disabled={loading}
            />
          </div>

          <div style={s.row}>
            <div style={{ flex: 1 }}>
              <label style={s.label}>Subject (optional)</label>
              <select
                style={s.select}
                value={subject}
                onChange={e => setSubject(e.target.value)}
                disabled={loading}
              >
                <option value="">Auto-detect from prompt</option>
                {SUBJECTS.slice(1).map(t => (
                  <option key={t} value={t}>
                    {t.replace(/_/g, ' ').replace(/\b\w/g, c => c.toUpperCase())}
                  </option>
                ))}
              </select>
            </div>

            <div style={s.gradeBox}>
              <label style={s.label}>
                <span style={s.gradeVal}>
                  <span>Grade level</span>
                  <span style={{ color: '#7c6fff', fontWeight: 700 }}>{grade}</span>
                </span>
              </label>
              <input
                type="range" min={1} max={12} step={1}
                value={grade}
                onChange={e => setGrade(Number(e.target.value))}
                style={s.slider}
                disabled={loading}
              />
              <div style={{ display: 'flex', justifyContent: 'space-between', fontSize: 11, color: '#4a4a6a' }}>
                <span>1</span><span>6</span><span>12</span>
              </div>
            </div>
          </div>

          <div style={s.actions}>
            <button type="submit" style={s.btn(true, !canGenerate)} disabled={!canGenerate}>
              {loading ? '⏳ Generating…' : '✨ Generate Game'}
            </button>
            <button
              type="button"
              style={s.btn(false, !hasScene || loading)}
              disabled={!hasScene || loading}
              onClick={onSave}
              title="Save to engine data/scenes/"
            >
              💾 Save
            </button>
            <button
              type="button"
              style={s.btn(false, !hasScene || loading)}
              disabled={!hasScene || loading}
              onClick={onDownload}
              title="Download as .json file"
            >
              ⬇ Download
            </button>
          </div>
        </form>
      </div>

      {/* Quick tips */}
      <div style={s.tip}>
        <strong style={{ color: '#94a3b8' }}>Quick tips</strong><br />
        • Be specific: <em>"teach photosynthesis — collect CO₂, H₂O, sunlight"</em><br />
        • Vague prompts use auto-detected or default theme assets<br />
        • Grade 1–4 = no enemies; Grade 5+ = up to 3 patrol enemies<br />
        • Saved scenes appear immediately in the C++ engine (F5 to reload)
      </div>
    </div>
  )
}
