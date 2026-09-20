# Channel Scan Feature Detailed Guide

This document provides detailed instructions for using the "Channel Scan" feature in TVMC.

## 📖 Feature Overview

Channel Scan is used for batch detection of IPTV streaming address availability. By configuring address templates, the software automatically expands all possible address combinations and uses FFmpeg to probe each address for availability.

## 🖥️ Interface Description

### Scan Address Area

| Control | Description |
|---------|-------------|
| **URL Radio Button** | Select URL input mode |
| **File Radio Button** | Select file input mode |
| **Input Field** | Enter URL template or file path |
| **... Button** | Select file in file mode |

### Scan Parameters Area

| Parameter | Description | Recommended |
|-----------|-------------|-------------|
| **Threads** | Concurrent scan threads | 8-16 |
| **Timeout(ms)** | Single address probe timeout | 500-10000 |
| **Auto Skip** | Skip subsequent IPs with existing success | Enabled |
| **Add TS** | Add "待整理 TS" prefix to results | Disabled |
| **Fine Scan** | Halve timeout on EIO errors for retry | Disabled |

### Button Area

| Button | Function |
|--------|----------|
| **Start** | Start scanning (clears previous results) |
| **Stop** | Stop current scan |
| **Clear** | Clear scan results |
| **Save** | Save scan results to file |

### Progress Display Area

| Display | Description |
|---------|-------------|
| **Status Bar** | Currently scanning URL |
| **Progress** | Scanned count / Total count |
| **Statistics** | Success count, Failure count |
| **Progress Bar** | Scan progress percentage |

### Results Area

Displays scan result logs, including:
- Discovered available addresses
- Resolution and frame rate information
- Usage help tips

## 📝 URL Template Syntax

### Basic Rules

URL templates use curly braces `{}` to define variable parts, supporting three expression methods:

#### 1. Values (separated by `#`)

```
{value1#value2#value3}
```

**Examples:**
```
{1#3#5}        → 1, 3, 5
{6000#8000}    → 6000, 8000
```

#### 2. Range (using `[start-end]`)

```
{[start-end]}
```

**Examples:**
```
{[1-10]}       → 1, 2, 3, ..., 10
{[08-10]}      → 08, 09, 10 (with leading zeros)
{[100-200]}    → 100, 101, ..., 200
```

#### 3. Mixed Usage

```
{value1#value2#[start-end]}
```

**Examples:**
```
{1#3#[5-7]}    → 1, 3, 5, 6, 7
{6000#[8000-8010]} → 6000, 8000, 8001, ..., 8010
```

### Nesting Rules

Maximum **3 levels of curly brace nesting** are supported. The system automatically expands all combinations.

**Example:**
```
http://192.168.1.1:12345/udp/239.49.{0#1#2}.{[1-255]}:{6000#8000}
```

Expands to:
```
http://192.168.1.1:12345/udp/239.49.0.1:6000
http://192.168.1.1:12345/udp/239.49.0.1:8000
http://192.168.1.1:12345/udp/239.49.0.2:6000
http://192.168.1.1:12345/udp/239.49.0.2:8000
...
http://192.168.1.1:12345/udp/239.49.2.255:8000
```

## 🔧 Use Cases

### Case 1: Scan LAN Multicast Addresses

**Requirement:** Scan all addresses in 239.49.0.x range, port 6000

**URL Template:**
```
http://192.168.1.1:12345/udp/239.49.0.{[1-255]}:6000
```

**Parameter Settings:**
- Threads: 8
- Timeout: 1000 (LAN)
- Auto Skip: Enabled

### Case 2: Scan Multiple Ports

**Requirement:** Scan multiple ports on 239.49.0.1

**URL Template:**
```
http://192.168.1.1:12345/udp/239.49.0.1:{6000#8000#8008}
```

**Parameter Settings:**
- Threads: 4
- Timeout: 1000

### Case 3: Batch Scan from File

**Requirement:** Scan addresses read from a file

**Steps:**
1. Prepare a text file with one address template per line
2. Select the **「File」** radio button
3. Click **「...」** to select the file
4. Click **「Start」**

**File Format Example:**
```
http://192.168.1.1:12345/udp/239.49.0.{[1-100]}:6000
http://192.168.1.1:12345/udp/239.49.1.{[1-100]}:6000
http://192.168.1.1:12345/udp/239.49.2.{[1-100]}:6000
```

## ⚙️ Parameter Tuning

### Thread Count Settings

| Scenario | Recommended | Description |
|----------|-------------|-------------|
| LAN + High-performance server | 16-32 | Server can handle high concurrency |
| LAN + Normal server | 8-16 | Balance speed and stability |
| WAN + udpxy | 4-8 | udpxy has limited connections |
| Uncertain | 8 | Default recommended value |

### Timeout Settings

| Scenario | Recommended | Description |
|----------|-------------|-------------|
| LAN direct | 500-1000 | Low network latency |
| LAN + udpxy | 1000-5000 | Need to wait for udpxy response |
| WAN | 5000-10000 | High network latency |
| Uncertain | 10000 | No timeout limit |

### Auto Skip

**When Enabled:**
- After finding a successful channel at an IP, subsequent addresses at that IP are skipped
- Suitable for scenarios where each IP hosts only one channel
- Greatly improves scanning efficiency

**When Disabled:**
- Scans all addresses without skipping
- Suitable for scenarios where one IP hosts multiple channels

### Fine Scan

**When Enabled:**
- On EIO errors, automatically halves timeout for retry
- Maximum 2 retries
- Suitable for unstable network conditions

**When Disabled:**
- Marks failures immediately on errors
- Suitable for stable network conditions

## 📊 Result Format

Successfully scanned addresses are displayed in the following format:

```
待整理 TS 720x576(25),http://172.28.0.2:25140/udp/239.49.0.1:8000
```

Format explanation:
- `待整理 TS`: Source type (controlled by "Add TS" option)
- `720x576`: Video resolution
- `(25)`: Frame rate
- `http://...`: Full address

## 💾 Save and Import

### Save Scan Results

1. After scanning completes, click the **「Save」** button
2. Choose save location and filename
3. File format is `.mc` (TVMC proprietary format)

### Import to Channel Management

1. Switch to the **「Settings」** tab
2. In the **「Import File」** section, select the saved `.mc` file
3. Click the **「Import」** button
4. After successful import, view and edit in **「Channel Edit」** tab

### Export to Other Formats

After importing, you can export to:
- **M3U**: Standard playlist
- **TXT**: Text format
- **XLS**: Excel spreadsheet
- **CSV**: Comma-separated values

## 🔄 Complete Workflow

```
1. Configure server address
   ↓
2. Configure multicast address template and ports
   ↓
3. Generate scan file
   ↓
4. Switch to scan page
   ↓
5. Set scan parameters
   ↓
6. Start scanning
   ↓
7. Wait for scan completion
   ↓
8. Save scan results
   ↓
9. Import to channel management
   ↓
10. Edit channel information
   ↓
11. Modify configuration, repeat steps 3-10
```

## ⚠️ Important Notes

1. **Address Count**: Templates may expand to a large number of addresses, control the range
2. **Server Load**: High-concurrency scanning may stress the server, set threads appropriately
3. **Network Environment**: Adjust timeout based on actual network conditions
4. **Result Saving**: Scan results are not auto-saved, save promptly
5. **Repeated Scanning**: Starting a new scan clears previous results

## 🐛 FAQ

### Q: Scan results are empty?

**Possible Causes:**
- Address template configuration error
- Server unreachable
- Timeout too short

**Solutions:**
- Check address template syntax
- Test server connectivity
- Increase timeout

### Q: Scanning is very slow?

**Possible Causes:**
- Timeout too long
- Too few threads
- High network latency

**Solutions:**
- Reduce timeout appropriately
- Increase thread count
- Check network environment

### Q: Scan was interrupted?

**Solution:**
- Click "Clear" to clear results
- Restart scanning
- Enable "Auto Skip" for efficiency

### Q: How to improve scanning efficiency?

**Recommendations:**
- Enable "Auto Skip"
- Set appropriate thread count
- Adjust timeout based on network
- Use file mode for batch scanning

---

**Version**: TVMC v2.2.1  
**Updated**: 2026-07-15
