import '../styles/Loader.css';

/**
 * Loading spinner component
 */
const Loader = ({ size = 'medium', message = 'Loading...' }) => {
  return (
    <div className="loader-container">
      <div className={`loader loader-${size}`}></div>
      {message && <p className="loader-text">{message}</p>}
    </div>
  );
};

export default Loader;
