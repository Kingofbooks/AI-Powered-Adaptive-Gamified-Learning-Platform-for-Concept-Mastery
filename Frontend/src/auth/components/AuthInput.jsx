import '../styles/Auth.css';

/**
 * AuthInput - Reusable input component
 */
function AuthInput({
  type = 'text',
  label,
  name,
  value,
  onChange,
  placeholder = '',
  error = '',
  required = false,
}) {
  return (
    <div className="form-group">
      <label htmlFor={name} className="form-label">
        {label}
        {required && <span className="required-star">*</span>}
      </label>
      <input
        type={type}
        id={name}
        name={name}
        value={value}
        onChange={onChange}
        placeholder={placeholder}
        className={`form-input ${error ? 'input-error' : ''}`}
      />
      {error && <p className="error-message">{error}</p>}
    </div>
  );
}

export default AuthInput;
