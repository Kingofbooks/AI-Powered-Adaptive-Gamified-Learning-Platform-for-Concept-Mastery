import '../styles/Table.css';
import AdminButton from './AdminButton';

function Table({ 
  columns = [], 
  data = [], 
  onEdit = null, 
  onDelete = null,
  isLoading = false
}) {
  if (isLoading) {
    return <div className="table-loading">Loading...</div>;
  }

  if (data.length === 0) {
    return <div className="table-empty">No data available</div>;
  }

  return (
    <div className="table-wrapper">
      <table className="admin-table">
        <thead>
          <tr>
            {columns.map((col) => (
              <th key={col.key} style={{ width: col.width }}>
                {col.label}
              </th>
            ))}
            {(onEdit || onDelete) && <th>Actions</th>}
          </tr>
        </thead>
        <tbody>
          {data.map((row, idx) => (
            <tr key={row.id || idx}>
              {columns.map((col) => (
                <td key={col.key}>
                  {col.render ? col.render(row[col.key], row) : row[col.key]}
                </td>
              ))}
              {(onEdit || onDelete) && (
                <td className="table-actions">
                  {onEdit && (
                    <AdminButton 
                      variant="secondary"
                      size="small"
                      onClick={() => onEdit(row)}
                    >
                      Edit
                    </AdminButton>
                  )}
                  {onDelete && (
                    <AdminButton 
                      variant="danger"
                      size="small"
                      onClick={() => {
                        if (confirm('Are you sure?')) {
                          onDelete(row.id);
                        }
                      }}
                    >
                      Delete
                    </AdminButton>
                  )}
                </td>
              )}
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
}

export default Table;
