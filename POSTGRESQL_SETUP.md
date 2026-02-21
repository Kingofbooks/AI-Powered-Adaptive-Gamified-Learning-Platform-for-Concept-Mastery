# PostgreSQL Setup Instructions

## Current Status

✅ **Completed**: Backend authentication system is **PostgreSQL-ready**
⏳ **Pending**: You're handling PostgreSQL setup separately

The system currently uses **SQLite** for testing and is fully compatible with **PostgreSQL** for production.

## How to Switch to PostgreSQL

### Step 1: Install PostgreSQL

#### Windows:
```bash
# Download from https://www.postgresql.org/download/windows/
# Run installer and follow setup
# Default port: 5432
```

#### macOS:
```bash
brew install postgresql@15
brew services start postgresql@15
```

#### Linux (Ubuntu/Debian):
```bash
sudo apt-get install postgresql postgresql-contrib
sudo service postgresql start
```

### Step 2: Create Database

```bash
# Connect to PostgreSQL
psql -U postgres

# Create database
CREATE DATABASE learning_platform;

# Create user (optional, for security)
CREATE USER learning_user WITH PASSWORD 'secure_password';
ALTER ROLE learning_user SET client_encoding TO 'utf8';
ALTER ROLE learning_user SET default_transaction_isolation TO 'read committed';
ALTER ROLE learning_user SET default_transaction_deferrable TO on;
ALTER ROLE learning_user SET timezone TO 'UTC';

# Grant privileges
GRANT ALL PRIVILEGES ON DATABASE learning_platform TO learning_user;

# Exit
\q
```

### Step 3: Update Backend .env

```env
# Change from SQLite:
# DATABASE_URL=sqlite:///./game_platform.db

# To PostgreSQL:
DATABASE_URL=postgresql://learning_user:secure_password@localhost:5432/learning_platform
```

### Step 4: Run Backend

```bash
cd Backend
python -m uvicorn api:app --reload --host 0.0.0.0 --port 8000
```

Tables will be created automatically on first run.

## Verification

### Connect to PostgreSQL and check tables:

```bash
psql -U learning_user -d learning_platform

# List tables
\dt

# Check users table
SELECT * FROM users;

# Exit
\q
```

## Database Schema

### Users Table (Auto-Created)
```
id              | integer | primary key
username        | string  | unique
email           | string  | unique
name            | string  |
password_hash   | string  |
role            | enum    | student, teacher, admin
created_at      | timestamp |
last_login      | timestamp |
is_active       | boolean |
level           | string  | beginner, intermediate, advanced
total_score     | integer |
total_games_played | integer |
college         | string  | (student only)
department      | string  | (student only)
year            | string  | (student only)
experience      | string  | (teacher only)
subjects        | json    | (teacher only)
classes         | json    | (teacher only)
```

### Other Auto-Created Tables
- `concepts` - Learning concepts
- `game_results` - Game performance records
- `progress` - Student progress tracking
- `learning_paths` - Custom learning paths
- `cache_data` - Cached game content

## Troubleshooting PostgreSQL

### "Connection refused"
```bash
# Check if PostgreSQL is running
sudo service postgresql status

# Start PostgreSQL
sudo service postgresql start
```

### "FATAL: password authentication failed"
```bash
# Make sure password matches .env
# Or use psql without password:
psql -U postgres -h localhost
```

### "Database does not exist"
```bash
# Create the database:
createdb -U postgres learning_platform
```

### "Role does not exist"
```bash
# Create user in PostgreSQL:
psql -U postgres
CREATE USER learning_user WITH PASSWORD 'password';
\q
```

## Backup & Restore

### Backup Database
```bash
pg_dump -U learning_user learning_platform > backup.sql
```

### Restore Database
```bash
psql -U learning_user -d learning_platform < backup.sql
```

## Connection Details

| Setting | Value |
|---------|-------|
| Host | localhost |
| Port | 5432 |
| Database | learning_platform |
| User | learning_user |
| Password | secure_password |
| URL Format | `postgresql://user:password@host:port/database` |

## Performance Optimization (Optional)

### Connection Pooling with pgBouncer

Install pgBouncer:
```bash
# macOS
brew install pgbouncer

# Linux
sudo apt-get install pgbouncer
```

## Migration from SQLite to PostgreSQL

If you have existing data in SQLite:

```bash
# Export from SQLite
sqlite3 game_platform.db ".dump" > sqlite_backup.sql

# Create PostgreSQL database
createdb learning_platform

# Import (manual - manual conversion may be needed due to SQL differences)
# For now, recommend starting fresh with PostgreSQL
```

## MongoDB Option (Alternative)

If you prefer MongoDB instead:

1. Install MongoDB
2. Update SQLAlchemy to use MongoDB driver
3. Update database connection string
4. Run migration

Contact for MongoDB setup if needed.

## Production Deployment

### Recommended Settings:

```env
# Production
DATABASE_URL=postgresql://learning_user:secure_password@db.example.com:5432/learning_platform_prod

# Connection pool
SQLALCHEMY_ECHO=False
SQLALCHEMY_POOL_SIZE=20
SQLALCHEMY_POOL_RECYCLE=3600
SQLALCHEMY_POOL_PRE_PING=True

# SSL
SQLALCHEMY_DATABASE_URI=postgresql+psycopg2://user:pass@host/dbname?sslmode=require
```

## Monitoring

### Check PostgreSQL Status
```bash
# Linux
sudo systemctl status postgresql

# macOS
brew services list | grep postgresql
```

### View Logs
```bash
# Check database logs
tail -f /var/log/postgresql/postgresql.log

# Or connect and check:
psql -U postgres -d learning_platform
SELECT * FROM pg_stat_statements LIMIT 10;
```

## Maintenance

### Regular Backup Schedule
```bash
# Add to crontab (daily backup at 2 AM)
0 2 * * * pg_dump -U learning_user learning_platform > /backups/learning_platform_$(date +\%Y\%m\%d).sql
```

### Vacuum & Analyze (optimize performance)
```bash
psql -U learning_user -d learning_platform
VACUUM ANALYZE;
\q
```

---

## Summary

✅ **Current**: SQLite (testing)
🔄 **Next**: PostgreSQL (production)
📝 **When ready**: Update DATABASE_URL in .env and restart backend

The authentication system will work exactly the same with PostgreSQL - no code changes needed! Just update the connection string.

For questions about PostgreSQL setup, refer to official docs:
https://www.postgresql.org/docs/
