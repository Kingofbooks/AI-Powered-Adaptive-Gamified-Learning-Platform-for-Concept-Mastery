import '../styles/Auth.css';

/**
 * AuthLayout - Shared layout for all auth pages
 * Provides consistent card-based design
 */
function AuthLayout({ 
  title, 
  subtitle = '', 
  children,
  footerText = '',
  footerLink = null
}) {
  return (
    <div className="auth-bg">
      <div className="auth-container">
        <div className="auth-card">
          <div className="auth-header">
            <h1 className="auth-title">{title}</h1>
            {subtitle && <p className="auth-subtitle">{subtitle}</p>}
          </div>

          <div className="auth-body">
            {children}
          </div>

          {footerText && (
            <div className="auth-footer">
              <p className="auth-footer-text">
                {footerText}
                {footerLink && (
                  <>
                    {' '}
                    <a href={footerLink.href} className="auth-footer-link">
                      {footerLink.text}
                    </a>
                  </>
                )}
              </p>
            </div>
          )}
        </div>

        <div className="auth-decoration">
          <div className="decoration-circle-1"></div>
          <div className="decoration-circle-2"></div>
        </div>
      </div>
    </div>
  );
}

export default AuthLayout;
