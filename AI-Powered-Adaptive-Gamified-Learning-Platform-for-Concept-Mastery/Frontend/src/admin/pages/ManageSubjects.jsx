import { useState, useEffect } from 'react';
import * as adminApi from '../api/adminApi';
import AdminNavbar from '../components/AdminNavbar';
import Table from '../components/Table';
import Modal from '../components/Modal';
import AdminButton from '../components/AdminButton';
import '../styles/AdminPages.css';

function ManageSubjects() {
  const [subjects, setSubjects] = useState([]);
  const [loading, setLoading] = useState(true);
  const [isModalOpen, setIsModalOpen] = useState(false);
  const [formData, setFormData] = useState({ name: '', description: '' });
  const [editingId, setEditingId] = useState(null);

  useEffect(() => {
    fetchSubjects();
  }, []);

  const fetchSubjects = async () => {
    try {
      const data = await adminApi.getSubjects();
      setSubjects(data);
    } catch (error) {
      console.error('Error fetching subjects:', error);
    } finally {
      setLoading(false);
    }
  };

  const handleOpenModal = (subject = null) => {
    if (subject) {
      setFormData({ name: subject.name, description: subject.description });
      setEditingId(subject.id);
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
    if (!formData.name.trim()) {
      alert('Subject name is required');
      return;
    }

    try {
      if (editingId) {
        await adminApi.updateSubject(editingId, formData);
      } else {
        await adminApi.addSubject(formData);
      }
      fetchSubjects();
      handleCloseModal();
    } catch (error) {
      console.error('Error saving subject:', error);
      alert('Failed to save subject');
    }
  };

  const handleDelete = async (id) => {
    try {
      await adminApi.deleteSubject(id);
      fetchSubjects();
    } catch (error) {
      console.error('Error deleting subject:', error);
      alert('Failed to delete subject');
    }
  };

  const columns = [
    { key: 'name', label: 'Subject Name' },
    { key: 'description', label: 'Description' },
    { key: 'questionCount', label: 'Questions' },
  ];

  return (
    <>
      <AdminNavbar />
      <div className="admin-container">
        <div className="admin-page-header">
          <h1>📚 Manage Subjects</h1>
          <AdminButton 
            variant="primary"
            onClick={() => handleOpenModal()}
          >
            ➕ Add Subject
          </AdminButton>
        </div>

        <Table
          columns={columns}
          data={subjects}
          isLoading={loading}
          onEdit={handleOpenModal}
          onDelete={handleDelete}
        />

        <Modal
          isOpen={isModalOpen}
          onClose={handleCloseModal}
          title={editingId ? 'Edit Subject' : 'Add New Subject'}
          onSubmit={handleSubmit}
          submitLabel={editingId ? 'Update' : 'Add'}
        >
          <div className="form-group">
            <label>Subject Name *</label>
            <input
              type="text"
              value={formData.name}
              onChange={(e) => setFormData({ ...formData, name: e.target.value })}
              placeholder="e.g., Java"
              className="form-input"
            />
          </div>
          <div className="form-group">
            <label>Description</label>
            <textarea
              value={formData.description}
              onChange={(e) => setFormData({ ...formData, description: e.target.value })}
              placeholder="Brief description of the subject"
              className="form-input"
              rows="3"
            />
          </div>
        </Modal>
      </div>
    </>
  );
}

export default ManageSubjects;
