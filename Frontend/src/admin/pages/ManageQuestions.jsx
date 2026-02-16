import { useState, useEffect } from 'react';
import * as adminApi from '../api/adminApi';
import AdminNavbar from '../components/AdminNavbar';
import Table from '../components/Table';
import Modal from '../components/Modal';
import AdminButton from '../components/AdminButton';
import '../styles/AdminPages.css';

function ManageQuestions() {
  const [subjects, setSubjects] = useState([]);
  const [concepts, setConcepts] = useState([]);
  const [questions, setQuestions] = useState([]);
  const [loading, setLoading] = useState(false);
  const [selectedSubject, setSelectedSubject] = useState(null);
  const [selectedConcept, setSelectedConcept] = useState(null);
  const [isModalOpen, setIsModalOpen] = useState(false);
  const [formData, setFormData] = useState({ 
    text: '', 
    difficulty: 'Medium'
  });
  const [editingId, setEditingId] = useState(null);

  useEffect(() => {
    fetchSubjects();
  }, []);

  useEffect(() => {
    if (selectedSubject) {
      fetchConcepts(selectedSubject);
      setSelectedConcept(null);
    }
  }, [selectedSubject]);

  useEffect(() => {
    if (selectedConcept) {
      fetchQuestions(selectedConcept);
    }
  }, [selectedConcept]);

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
    try {
      const data = await adminApi.getConceptsBySubject(subjectId);
      setConcepts(data);
      if (data.length > 0) {
        setSelectedConcept(data[0].id);
      }
    } catch (error) {
      console.error('Error fetching concepts:', error);
    }
  };

  const fetchQuestions = async (conceptId) => {
    setLoading(true);
    try {
      const data = await adminApi.getQuestionsByConceptId(conceptId);
      setQuestions(data);
    } catch (error) {
      console.error('Error fetching questions:', error);
    } finally {
      setLoading(false);
    }
  };

  const handleOpenModal = (question = null) => {
    if (question) {
      setFormData({ text: question.text, difficulty: question.difficulty });
      setEditingId(question.id);
    } else {
      setFormData({ text: '', difficulty: 'Medium' });
      setEditingId(null);
    }
    setIsModalOpen(true);
  };

  const handleCloseModal = () => {
    setIsModalOpen(false);
    setFormData({ text: '', difficulty: 'Medium' });
    setEditingId(null);
  };

  const handleSubmit = async () => {
    if (!formData.text.trim() || !selectedConcept) {
      alert('Question text and concept are required');
      return;
    }

    try {
      if (editingId) {
        await adminApi.updateQuestion(editingId, formData);
      } else {
        await adminApi.addQuestion({
          ...formData,
          conceptId: selectedConcept,
          subjectId: selectedSubject,
        });
      }
      fetchQuestions(selectedConcept);
      handleCloseModal();
    } catch (error) {
      console.error('Error saving question:', error);
      alert('Failed to save question');
    }
  };

  const handleDelete = async (id) => {
    try {
      await adminApi.deleteQuestion(id);
      fetchQuestions(selectedConcept);
    } catch (error) {
      console.error('Error deleting question:', error);
      alert('Failed to delete question');
    }
  };

  const columns = [
    { key: 'text', label: 'Question', width: '50%' },
    { key: 'difficulty', label: 'Difficulty' },
    { key: 'answered', label: 'Times Answered' },
  ];

  return (
    <>
      <AdminNavbar />
      <div className="admin-container">
        <div className="admin-page-header">
          <h1>❓ Manage Questions</h1>
          <AdminButton 
            variant="primary"
            onClick={() => handleOpenModal()}
            disabled={!selectedConcept}
          >
            ➕ Add Question
          </AdminButton>
        </div>

        <div className="filter-section">
          <div className="filter-group">
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

          <div className="filter-group">
            <label className="filter-label">Select Concept:</label>
            <select
              value={selectedConcept || ''}
              onChange={(e) => setSelectedConcept(Number(e.target.value))}
              className="filter-select"
            >
              <option value="">-- Choose a concept --</option>
              {concepts.map((concept) => (
                <option key={concept.id} value={concept.id}>
                  {concept.name}
                </option>
              ))}
            </select>
          </div>
        </div>

        {selectedConcept && (
          <Table
            columns={columns}
            data={questions}
            isLoading={loading}
            onEdit={handleOpenModal}
            onDelete={handleDelete}
          />
        )}

        {!selectedConcept && (
          <div className="table-empty">Please select a concept to view and manage questions</div>
        )}

        <Modal
          isOpen={isModalOpen}
          onClose={handleCloseModal}
          title={editingId ? 'Edit Question' : 'Add New Question'}
          onSubmit={handleSubmit}
          submitLabel={editingId ? 'Update' : 'Add'}
        >
          <div className="form-group">
            <label>Question Text *</label>
            <textarea
              value={formData.text}
              onChange={(e) => setFormData({ ...formData, text: e.target.value })}
              placeholder="Enter the question"
              className="form-input"
              rows="4"
            />
          </div>
          <div className="form-group">
            <label>Difficulty Level</label>
            <select
              value={formData.difficulty}
              onChange={(e) => setFormData({ ...formData, difficulty: e.target.value })}
              className="form-input"
            >
              <option>Easy</option>
              <option>Medium</option>
              <option>Hard</option>
            </select>
          </div>
        </Modal>
      </div>
    </>
  );
}

export default ManageQuestions;
