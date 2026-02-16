import { useState } from 'react';
import '../styles/Modal.css';
import AdminButton from './AdminButton';

function Modal({ 
  isOpen, 
  onClose, 
  title, 
  children,
  onSubmit,
  submitLabel = 'Save'
}) {
  if (!isOpen) return null;

  const handleBackdropClick = (e) => {
    if (e.target.classList.contains('modal-backdrop')) {
      onClose();
    }
  };

  return (
    <div className="modal-backdrop" onClick={handleBackdropClick}>
      <div className="modal-content">
        <div className="modal-header">
          <h2>{title}</h2>
          <button 
            className="modal-close-btn" 
            onClick={onClose}
            aria-label="Close modal"
          >
            ×
          </button>
        </div>
        <div className="modal-body">
          {children}
        </div>
        <div className="modal-footer">
          <AdminButton 
            variant="secondary" 
            onClick={onClose}
          >
            Cancel
          </AdminButton>
          <AdminButton 
            variant="primary" 
            onClick={onSubmit}
          >
            {submitLabel}
          </AdminButton>
        </div>
      </div>
    </div>
  );
}

export default Modal;
