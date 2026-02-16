import '../styles/StatCard.css';

function StatCard({ 
  icon, 
  label, 
  value, 
  subtitle = '', 
  color = 'blue',
  onClick = null
}) {
  return (
    <div 
      className={`stat-card stat-card-${color}`}
      onClick={onClick}
      style={{ cursor: onClick ? 'pointer' : 'default' }}
    >
      <div className="stat-icon">{icon}</div>
      <div className="stat-info">
        <p className="stat-label">{label}</p>
        <p className="stat-value">{value}</p>
        {subtitle && <p className="stat-subtitle">{subtitle}</p>}
      </div>
    </div>
  );
}

export default StatCard;
