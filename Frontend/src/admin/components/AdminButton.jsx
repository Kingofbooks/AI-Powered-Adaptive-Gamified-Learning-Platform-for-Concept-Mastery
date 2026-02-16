import '../styles/AdminButton.css';

function AdminButton({ 
  children, 
  type = 'button', 
  variant = 'primary',
  size = 'medium',
  onClick = null,
  disabled = false,
  className = ''
}) {
  return (
    <button
      type={type}
      onClick={onClick}
      disabled={disabled}
      className={`admin-btn admin-btn-${variant} admin-btn-${size} ${className}`}
    >
      {children}
    </button>
  );
}

export default AdminButton;
