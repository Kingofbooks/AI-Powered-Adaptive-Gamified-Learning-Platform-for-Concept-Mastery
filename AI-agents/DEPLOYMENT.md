# Deployment Guide for Teacher AI Agent

This guide covers different ways to deploy the Teacher AI Agent.

## Table of Contents
1. Local Development
2. Docker Deployment
3. Cloud Deployment (AWS, GCP, Azure)
4. Production Considerations

---

## 1. Local Development

### Windows

```bash
# Run the startup script
start.bat

# Or manually:
python -m venv venv
venv\Scripts\activate
pip install -r requirements.txt
python main.py
```

### macOS/Linux

```bash
# Run the startup script
chmod +x start.sh
./start.sh

# Or manually:
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
python main.py
```

The server will be available at `http://localhost:8000`

---

## 2. Docker Deployment

### Prerequisites
- Docker installed
- Docker Compose installed

### Build and Run

```bash
# Build the image
docker build -t teacher-ai-agent:latest .

# Run the container
docker run -d \
  --name teacher-ai-agent \
  -p 8000:8000 \
  -e OPENAI_API_KEY=your_api_key \
  teacher-ai-agent:latest
```

### Using Docker Compose

```bash
# Create .env file
echo "OPENAI_API_KEY=your_api_key" > .env

# Start the service
docker-compose up -d

# Check logs
docker-compose logs -f teacher-ai-agent

# Stop the service
docker-compose down
```

### Docker Image Locations
- Local: `http://localhost:8000`
- Within Docker network: `http://teacher-ai-agent:8000`

---

## 3. Cloud Deployment

### AWS (using Lambda + API Gateway)

1. **Package the application:**
```bash
# Create a deployment package
pip install -r requirements.txt -t ./package
cp -r . ./package/
cd package
zip -r function.zip .
```

2. **Upload to Lambda:**
- Use AWS Lambda console or AWS CLI
- Runtime: Python 3.11
- Handler: `main.app`
- Timeout: 120 seconds
- Memory: 1024 MB recommended

3. **Create API Gateway:**
- Trigger: API Gateway
- Route: `/generate-module` and others
- Method: POST

### Google Cloud Run

```bash
# Build and push to Google Container Registry
gcloud builds submit --tag gcr.io/PROJECT_ID/teacher-ai-agent .

# Deploy
gcloud run deploy teacher-ai-agent \
  --image gcr.io/PROJECT_ID/teacher-ai-agent \
  --platform managed \
  --region us-central1 \
  --set-env-vars OPENAI_API_KEY=your_api_key \
  --allow-unauthenticated
```

### Azure Container Instances

```bash
# Create resource group
az group create --name teacher-ai --location eastus

# Deploy container
az container create \
  --resource-group teacher-ai \
  --name teacher-ai-agent \
  --image teacher-ai-agent:latest \
  --environment-variables OPENAI_API_KEY=your_api_key \
  --ports 8000 \
  --cpu 1 \
  --memory 1
```

### Heroku

```bash
# Create Heroku app
heroku create teacher-ai-agent

# Add buildpack
heroku buildpacks:add heroku/python

# Set environment variable
heroku config:set OPENAI_API_KEY=your_api_key

# Deploy
git push heroku main

# View logs
heroku logs --tail
```

---

## 4. Production Considerations

### Security

```python
# Use HTTPS only
CORS_ORIGINS = ["https://yourdomain.com"]  # Not "*"

# Add authentication
from fastapi.security import HTTPBearer
security = HTTPBearer()

@app.post("/generate-module")
async def generate_module(request: LearningModuleRequest, credentials: HTTPAuthCredentials = Depends(security)):
    # Validate credentials
    ...
```

### Rate Limiting

```python
from slowapi import Limiter
from slowapi.util import get_remote_address

limiter = Limiter(key_func=get_remote_address)
app.state.limiter = limiter

@app.post("/generate-module")
@limiter.limit("30/minute")
async def generate_learning_module(request: LearningModuleRequest):
    ...
```

### Database Integration

```python
# Add SQLAlchemy for storing modules
from sqlalchemy import create_engine, Column, String, DateTime
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker

DATABASE_URL = "postgresql://user:password@localhost/teacher_ai"
engine = create_engine(DATABASE_URL)
SessionLocal = sessionmaker(bind=engine)
Base = declarative_base()

class GeneratedModule(Base):
    __tablename__ = "modules"
    
    id = Column(String, primary_key=True)
    concept = Column(String)
    level = Column(String)
    content = Column(JSON)
    created_at = Column(DateTime)
```

### Monitoring and Logging

```python
import logging
from pythonjsonlogger import jsonlogger

# JSON logging for better analysis
logger = logging.getLogger()
handler = logging.StreamHandler()
formatter = jsonlogger.JsonFormatter()
handler.setFormatter(formatter)
logger.addHandler(handler)

# Log all requests
@app.middleware("http")
async def log_requests(request, call_next):
    logger.info({
        "endpoint": request.url.path,
        "method": request.method,
        "timestamp": datetime.now().isoformat()
    })
    response = await call_next(request)
    return response
```

### Caching Strategy

```python
from functools import lru_cache
from redis import Redis

redis_client = Redis(host='localhost', port=6379)

@app.get("/cached-module/{concept}")
async def get_cached_module(concept: str):
    # Check cache first
    cached = redis_client.get(f"module:{concept}")
    if cached:
        return json.loads(cached)
    
    # Generate new module
    module = generate_module(concept)
    
    # Cache for 1 hour
    redis_client.setex(f"module:{concept}", 3600, json.dumps(module))
    return module
```

### Load Balancing

```bash
# Using Nginx as reverse proxy
upstream api_servers {
    server localhost:8000;
    server localhost:8001;
    server localhost:8002;
}

server {
    listen 80;
    location / {
        proxy_pass http://api_servers;
    }
}
```

### Environment Configuration

```bash
# Use environment-specific configs
.env.development
.env.staging
.env.production

# Load appropriate config
ENV=$(python -c "import os; print(os.getenv('ENVIRONMENT', 'development'))")
source .env.${ENV}
```

### Health Checks

```python
@app.get("/health/ready")
async def readiness_check():
    """Check if service is ready to serve traffic"""
    try:
        # Check API connectivity
        response = client.messages.create(
            model=LLM_MODEL,
            max_tokens=10,
            messages=[{"role": "user", "content": "OK"}]
        )
        return {"status": "ready"}
    except:
        return {"status": "not_ready"}, 503

@app.get("/health/live")
async def liveness_check():
    """Simple liveness check"""
    return {"status": "alive"}
```

### Scaling Recommendations

- **Vertical Scaling**: Increase server resources (CPU, RAM)
- **Horizontal Scaling**: Use load balancing with multiple instances
- **Queue System**: Use Celery + Redis for async job processing
- **Caching**: Cache frequently requested modules
- **CDN**: Serve static files through CDN

### Cost Optimization

1. **API Token Optimization**
   - Shorter prompts where possible
   - Reuse prompts for similar concepts
   - Implement caching aggressively

2. **Infrastructure**
   - Use auto-scaling groups
   - Implement request batching
   - Use serverless for variable workloads

3. **Monitoring**
   - Track API usage and costs
   - Optimize prompts based on token usage
   - Monitor error rates

---

## Deployment Checklist

- [ ] Set up OPENAI_API_KEY securely
- [ ] Configure CORS for your domain
- [ ] Enable HTTPS/TLS
- [ ] Set up rate limiting
- [ ] Configure logging and monitoring
- [ ] Set up automated backups
- [ ] Test health check endpoints
- [ ] Configure auto-restart policies
- [ ] Set up alerting for errors
- [ ] Document rollback procedures
- [ ] Test the deployment
- [ ] Monitor performance metrics

---

## Troubleshooting Deployments

### Issue: API Key Not Recognized
```bash
# Verify the key is set correctly
echo $OPENAI_API_KEY
```

### Issue: Port Already in Use
```bash
# Kill process on port 8000
lsof -ti:8000 | xargs kill -9
```

### Issue: Memory Issues
- Increase allocated RAM
- Implement request queuing
- Reduce model context window

### Issue: Slow Responses
- Check network latency
- Implement caching
- Scale horizontally
- Optimize prompts

---

For more information, see README.md
