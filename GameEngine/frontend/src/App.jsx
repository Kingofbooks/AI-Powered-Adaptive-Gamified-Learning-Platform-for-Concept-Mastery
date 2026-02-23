import React, { useState, useEffect, useCallback } from 'react'
import GeneratorPanel from './components/GeneratorPanel'
import SceneList from './components/SceneList'
import JsonViewer from './components/JsonViewer'

const API = ''   // empty = use Vite proxy (same origin in dev)

const styles = {
  app: {
    display: 'grid',
    gridTemplateColumns: '320px 1fr 360px',
    gridTemplateRows: '60px 1fr',
    height: '100vh',
    gap: 0,
  },
  header: {
    gridColumn: '1 / -1',
    background: '#1a1a2e',
    borderBottom: '1px solid #2d2d4a',
    display: 'flex',
    alignItems: 'center',
    padding: '0 24px',
    gap: 12,
  },
  logo: { fontSize: 22, fontWeight: 700, color: '#7c6fff' },
  subtitle: { fontSize: 13, color: '#94a3b8' },
  sidebar: {
    background: '#13131f',
    borderRight: '1px solid #2d2d4a',
    overflow: 'hidden',
    display: 'flex',
    flexDirection: 'column',
  },
  main: {
    background: '#0f0f1a',
    overflow: 'auto',
    padding: 24,
    display: 'flex',
    flexDirection: 'column',
    gap: 16,
  },
  right: {
    background: '#13131f',
    borderLeft: '1px solid #2d2d4a',
    overflow: 'hidden',
    display: 'flex',
    flexDirection: 'column',
  },
}

export default function App() {
  const [scene, setScene]   = useState(null)      // generated scene object
  const [scenes, setScenes] = useState([])         // list of saved scenes
  const [status, setStatus] = useState('')         // status bar message
  const [loading, setLoading] = useState(false)

  // Load saved scenes list
  const refreshScenes = useCallback(async () => {
    try {
      const res = await fetch(`${API}/api/scenes`)
      const data = await res.json()
      setScenes(data.scenes || [])
    } catch {
      setScenes([])
    }
  }, [])

  useEffect(() => { refreshScenes() }, [refreshScenes])

  // Generate a new scene via AI
  const handleGenerate = async ({ prompt, subject, grade }) => {
    setLoading(true)
    setStatus('Generating scene with AI…')
    setScene(null)
    try {
      const res = await fetch(`${API}/api/generate`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ prompt, subject, grade }),
      })
      if (!res.ok) {
        const err = await res.json()
        throw new Error(err.detail || `HTTP ${res.status}`)
      }
      const data = await res.json()
      setScene(data)
      setStatus(`✓ Scene generated: ${data.scene_meta?.title || 'Untitled'}`)
    } catch (e) {
      setStatus(`✗ Error: ${e.message}`)
    } finally {
      setLoading(false)
    }
  }

  // Save current scene to data/scenes/
  const handleSave = async () => {
    if (!scene) return
    setStatus('Saving…')
    try {
      const res = await fetch(`${API}/api/save`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ scene }),
      })
      if (!res.ok) throw new Error((await res.json()).detail)
      const data = await res.json()
      setStatus(`✓ Saved to ${data.saved}`)
      refreshScenes()
    } catch (e) {
      setStatus(`✗ Save failed: ${e.message}`)
    }
  }

  // Download JSON to browser
  const handleDownload = () => {
    if (!scene) return
    const blob = new Blob([JSON.stringify(scene, null, 2)], { type: 'application/json' })
    const url = URL.createObjectURL(blob)
    const a = document.createElement('a')
    a.href = url
    a.download = `${scene.scene_meta?.id || 'scene'}.json`
    a.click()
    URL.revokeObjectURL(url)
  }

  // Load a saved scene into the viewer
  const handleLoadScene = async (sceneFile) => {
    try {
      const res = await fetch(`${API}/api/scenes`)
      const data = await res.json()
      const found = data.scenes?.find(s => s.file === sceneFile)
      if (found) setStatus(`Loaded: ${found.title}`)
    } catch { /* ignore */ }
  }

  // Delete a saved scene
  const handleDeleteScene = async (sceneId) => {
    if (!window.confirm(`Delete scene "${sceneId}"?`)) return
    try {
      const res = await fetch(`${API}/api/scenes/${sceneId}`, { method: 'DELETE' })
      if (!res.ok) throw new Error((await res.json()).detail)
      setStatus(`✓ Deleted: ${sceneId}`)
      refreshScenes()
    } catch (e) {
      setStatus(`✗ Delete failed: ${e.message}`)
    }
  }

  return (
    <div style={styles.app}>
      {/* Header */}
      <header style={styles.header}>
        <span style={styles.logo}>EduEngine</span>
        <span style={styles.subtitle}>AI-Powered Educational Game Generator</span>
        {status && (
          <span style={{
            marginLeft: 'auto',
            fontSize: 13,
            color: status.startsWith('✗') ? '#f87171' : '#86efac',
            maxWidth: 400,
            overflow: 'hidden',
            textOverflow: 'ellipsis',
            whiteSpace: 'nowrap',
          }}>
            {status}
          </span>
        )}
      </header>

      {/* Left sidebar — scene list */}
      <aside style={styles.sidebar}>
        <SceneList
          scenes={scenes}
          onLoad={handleLoadScene}
          onDelete={handleDeleteScene}
          onRefresh={refreshScenes}
        />
      </aside>

      {/* Main — generator form */}
      <main style={styles.main}>
        <GeneratorPanel
          onGenerate={handleGenerate}
          onSave={handleSave}
          onDownload={handleDownload}
          loading={loading}
          hasScene={!!scene}
        />
      </main>

      {/* Right panel — JSON viewer */}
      <aside style={styles.right}>
        <JsonViewer scene={scene} />
      </aside>
    </div>
  )
}
