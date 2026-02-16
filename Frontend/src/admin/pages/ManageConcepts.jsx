import { useState, useEffect } from 'react';
import * as adminApi from '../api/adminApi';
import AdminNavbar from '../components/AdminNavbar';
import Table from '../components/Table';
import Modal from '../components/Modal';
import AdminButton from '../components/AdminButton';
import '../styles/AdminPages.css';

function ManageConcepts() {
  const [subjects, setSubjects] = useState([]);
  const [concepts, setConcepts] = useState([]);
  const [loading, setLoading] = useState(false);
  const [selectedSubject, setSelectedSubject] = useState(null);
  const [isModalOpen, setIsModalOpen] = useState(false);
  const [formData, setFormData] = useState({ name: '', description: '' });
  const [editingId, setEditingId] = useState(null);

  useEffect(() => {
    fetchSubjects();
  }, []);

  useEffect(() => {
    if (selectedSubject) {
      fetchConcepts(selectedSubject);
    }
  }, [selectedSubject]);

  const fetchSubjects = async () => {
    try {
      const data = await adminApi.getSubjects();
      setSubjects(data);
      if (data.length > 0) {
        setSelectedSubject(data[0].id);
      }
    } catch (error) {
      console.error('Error fetching subjects:', error);
    }
  };

  const fetchConcepts = async (subjectId) => {
    setLoading(true);
    try {
      const data = await adminApi.getConceptsBySubject(subjectId);
      setConcepts(data);
    } catch (error) {
      console.error('Error fetching concepts:', error);
    } finally {
      setLoading(false);
    }
  };

  const handleOpenModal = (concept = null) => {
    if (concept) {
      setFormData({ name: concept.name, description: concept.description });
      setEditingId(concept.id);
    } else {
      setFormData({ name: '', description: '' });
      setEditingId(null);
    }
    setIsModalOpen(true);
  };

  const handleCloseModal = () => {
    setIsModalOpen(false);
    setFormData({ name: '', description: '' });
    setEditingId(null);
  };

  const handleSubmit = async () => {
    if (!formData.name.trim() || !selectedSubject) {
      alert('Concept name and subject are required');
      return;
    }

    try {
      if (editingId) {
        await adminApi.updateConcept(editingId, formData);
      } else {
        await adminApi.addConcept({
          ...formData,
          subjectId: selectedSubject,
        });
      }
      fetchConcepts(selectedSubject);
      handleCloseModal();
    } catch (error) {
      console.error('Error saving concept:', error);
      alert('Failed to save concept');
    }
  };

  const handleDelete = async (id) => {
    try {
      await adminApi.deleteConcept(id);
      fetchConcepts(selectedSubject);
    } catch (error) {
      console.error('Error deleting concept:', error);
      alert('Failed to delete concept');
    }
  };

  const columns = [
    { key: 'name', label: 'Concept Name' },
    { key: 'description', label: 'Description' },
  ];

  return (
    <>
      <AdminNavbar />
      <div className="admin-container">
        <div className="admin-page-header">
          <h1>💡 Manage Concepts</h1>
          <AdminButton 
            variant="primary"
            onClick={() => handleOpenModal()}
          >
            ➕ Add Concept
          </AdminButton>
        </div>

        <div className="filter-section">
          <label className="filter-label">Select Subject:</label>
          <select
            value={selectedSubject || ''}
            onChange={(e) => setSelectedSubject(Number(e.target.value))}
            className="filter-select"
          >
            {subjects.map((subject) => (
              <option key={subject.id} value={subject.id}>
                {subject.name}
              </option>
            ))}
          </select>
        </div>

        <Table
          columns={columns}
          data={concepts}
          isLoading={loading}
          onEdit={handleOpenModal}
          onDelete={handleDelete}
        />

        <Modal
          isOpen={isModalOpen}
          onClose={handleCloseModal}
          title={editingId ? 'Edit Concept' : 'Add New Concept'}
          onSubmit={handleSubmit}
          submitLabel={editingId ? 'Update' : 'Add'}
        >
          <div className="form-group">
            <label>Concept Name *</label>
            <input
              type="text"
              value={formData.name}
              onChange={(e) => setFormData({ ...formData, name: e.target.value })}
              placeholder="e.g., OOP Concepts"
              className="form-input"
            />
          </div>
          <div className="form-group">
            <label>Description</label>
            <textarea
              value={formData.description}
              onChange={(e) => setFormData({ ...formData, description: e.target.value })}
              placeholder="Brief description of the concept"
              className="form-input"
              rows="3"
            />
          </div>
        </Modal>
      </div>
    </>
  );
}

export default ManageConcepts;
