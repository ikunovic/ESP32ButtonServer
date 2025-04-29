document.addEventListener('DOMContentLoaded', function() {
    // DOM Elements
    const ipAddressInput = document.getElementById('ipAddress');
    const connectButton = document.getElementById('connect');
    const connectionStatus = document.getElementById('connection-status');
    const controlPanel = document.getElementById('control-panel');
    const ledStatus = document.getElementById('led-status');
    const toggleLedButton = document.getElementById('toggle-led');
    
    // State
    let espIpAddress = '';
    let isConnected = false;
    
    // Initialize
    loadSavedIpAddress();
    
    // Event Listeners
    connectButton.addEventListener('click', handleConnect);
    toggleLedButton.addEventListener('click', handleToggleLed);
    
    // Functions
    function loadSavedIpAddress() {
        const savedIp = localStorage.getItem('espIpAddress');
        if (savedIp) {
            ipAddressInput.value = savedIp;
        }
    }
    
    function handleConnect() {
        espIpAddress = ipAddressInput.value.trim();
        
        if (!espIpAddress) {
            showError('Please enter an IP address');
            return;
        }
        
        // Store IP for future use
        localStorage.setItem('espIpAddress', espIpAddress);
        
        // Test connection
        testConnection()
            .then(() => {
                setConnectedState();
                getLedStatus();
            })
            .catch(error => {
                setDisconnectedState();
                showError(`Connection failed: ${error.message}`);
            });
    }
    
    function testConnection() {
        return new Promise((resolve, reject) => {
            fetch(`http://${espIpAddress}/api/status`)
                .then(response => {
                    if (!response.ok) {
                        throw new Error(`HTTP error ${response.status}`);
                    }
                    return response.json();
                })
                .then(data => {
                    console.log('Connection successful:', data);
                    resolve(data);
                })
                .catch(error => {
                    console.error('Connection failed:', error);
                    reject(error);
                });
        });
    }
    
    function getLedStatus() {
        if (!isConnected) return;
        
        fetch(`http://${espIpAddress}/api/led-status`)
            .then(response => {
                if (!response.ok) throw new Error(`HTTP error ${response.status}`);
                return response.json();
            })
            .then(data => {
                updateLedStatusUI(data.ledState);
            })
            .catch(error => {
                console.error('Failed to get LED status:', error);
                showError('Failed to get LED status');
            });
    }
    
    function handleToggleLed() {
        if (!isConnected) return;
        
        fetch(`http://${espIpAddress}/api/toggle-led`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            }
        })
        .then(response => {
            if (!response.ok) throw new Error(`HTTP error ${response.status}`);
            return response.json();
        })
        .then(data => {
            updateLedStatusUI(data.ledState);
            console.log(data.message);
        })
        .catch(error => {
            console.error('Failed to toggle LED:', error);
            showError('Failed to toggle LED');
        });
    }
    
    function updateLedStatusUI(isOn) {
        if (isOn) {
            ledStatus.textContent = 'ON';
            ledStatus.className = 'on';
            toggleLedButton.textContent = 'Turn LED OFF';
            toggleLedButton.className = 'btn danger';
        } else {
            ledStatus.textContent = 'OFF';
            ledStatus.className = 'off';
            toggleLedButton.textContent = 'Turn LED ON';
            toggleLedButton.className = 'btn success';
        }
    }
    
    function setConnectedState() {
        isConnected = true;
        connectionStatus.textContent = 'Connected';
        connectionStatus.className = 'connected';
        controlPanel.classList.remove('hidden');
    }
    
    function setDisconnectedState() {
        isConnected = false;
        connectionStatus.textContent = 'Disconnected';
        connectionStatus.className = 'error';
        controlPanel.classList.add('hidden');
    }
    
    function showError(message) {
        connectionStatus.textContent = message;
        connectionStatus.className = 'error';
        setTimeout(() => {
            connectionStatus.textContent = isConnected ? 'Connected' : 'Disconnected';
            connectionStatus.className = isConnected ? 'connected' : 'error';
        }, 3000);
    }
}); 