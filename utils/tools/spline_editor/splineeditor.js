const canvas = document.getElementById('splineCanvas');
const ctx = canvas.getContext('2d');
canvas.width = window.innerWidth * 0.8;
canvas.height = window.innerHeight * 0.8;

const points = [];

canvas.addEventListener('click', (e) => {
    const rect = canvas.getBoundingClientRect();
    const x = (e.clientX - rect.left) / canvas.width;
    const y = (e.clientY - rect.top) / canvas.height;
    points.push({ x, y });
    drawSpline();
});

canvas.addEventListener('contextmenu', (e) => {
    e.preventDefault();
    const rect = canvas.getBoundingClientRect();
    const x = (e.clientX - rect.left) / canvas.width;
    const y = (e.clientY - rect.top) / canvas.height;
    let closestPoint = null;
    let minDistance = Infinity;

    for (let i = 0; i < points.length; i++) {
        const dx = points[i].x - x;
        const dy = points[i].y - y;
        const distance = Math.sqrt(dx * dx + dy * dy);

        if (distance < minDistance) {
            minDistance = distance;
            closestPoint = i;
        }
    }

    if (minDistance < 10 / canvas.width) {
        points.splice(closestPoint, 1);
        drawSpline();
    }
});

document.getElementById('exportJson').addEventListener('click', () => {
    const json = JSON.stringify(points);

    // Use the Clipboard API to write the JSON data to the clipboard
    navigator.clipboard.writeText(json).then(() => {
        alert('Spline data copied to clipboard.');
    }).catch(err => {
        console.error('Error copying spline data to clipboard:', err);
        alert('Error copying spline data to clipboard.');
    });
});

function drawGrid() {
    const step = 0.1;

    ctx.strokeStyle = '#ccc';
    ctx.lineWidth = 1;
    ctx.beginPath();

    for (let i = step; i < 1; i += step) {
        // Vertical lines
        const x = i * canvas.width;
        ctx.moveTo(x, 0);
        ctx.lineTo(x, canvas.height);

        // Horizontal lines
        const y = i * canvas.height;
        ctx.moveTo(0, y);
        ctx.lineTo(canvas.width, y);
    }

    ctx.stroke();

    ctx.font = '12px Arial';
    ctx.fillStyle = 'black';
    ctx.textAlign = 'center';
    ctx.textBaseline = 'top';

    for (let i = step; i < 1; i += step) {
        const x = i * canvas.width;
        const y = i * canvas.height;
        ctx.fillText(i.toFixed(1), x, 0);
        ctx.fillText(i.toFixed(1), 0, y);
    }
}


function drawSpline() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    drawGrid();
    ctx.beginPath();

    for (let i = 0; i < points.length; i++) {
        const { x, y } = points[i];
        const screenX = x * canvas.width;
        const screenY = y * canvas.height;

        ctx.arc(screenX, screenY, 5, 0, 2 * Math.PI);
        ctx.fillStyle = 'transparent';
        ctx.fill();

        if (i === 0) {
            ctx.moveTo(screenX, screenY);
        } else {
            const prevScreenX = points[i - 1].x * canvas.width;
            const prevScreenY = points[i - 1].y * canvas.height;
            const controlX = (prevScreenX + screenX) / 2;
            const controlY = (prevScreenY + screenY) / 2;

            ctx.quadraticCurveTo(controlX, controlY, screenX, screenY);
        }
    }

    ctx.strokeStyle = 'black';
    ctx.lineWidth = 2;
    ctx.stroke();
}
