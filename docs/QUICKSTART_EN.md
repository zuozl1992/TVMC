# TVMC Quick Start Guide

This guide will help you quickly get started with TVMC, covering the complete workflow from configuration to scanning to channel management.

## 📋 Workflow Overview

```
Configuration → Generate Scan File → Start Scan → Save Results → Import Results → Channel Edit → Repeat
```

## 🚀 Quick Start

### Step 1: Software Configuration

1. Launch TVMC and switch to the **「Settings」** tab
2. In the **「Server Config」** section, fill in:
   - **Unicast URL**: Your streaming server address (e.g., `http://192.168.1.1:12345`)
   - **Logo URL**: Logo image server address (optional)
   - **Protocol**: Select `udp` or `rtp`
3. In the **「Generate Scan File」** section, configure:
   - **Multicast**: Multicast address template (e.g., `239.49.{0#1#2}`)
   - **Port**: Port number (e.g., `6000#8000#8008`)

> 💡 **Address Template Syntax**:
> - Values: `1#3` means 1 and 3
> - Range: `[8-10]` means 8, 9, 10
> - Mixed: `1#3#[5-7]` means 1, 3, 5, 6, 7

### Step 2: Generate Scan File

1. In the **「Settings」** tab's **「Generate Scan File」** section
2. Click the **「Generate Scan File」** button
3. Choose save location and filename
4. After successful generation, click **「Yes」** to auto-navigate to the scan page

### Step 3: Start Scanning

1. Switch to the **「Channel Scan」** tab (skip if already navigated)
2. Confirm the **「File」** radio button is selected and file path is filled
3. Adjust scan parameters:
   - **Threads**: Recommended 8-16 (adjust based on server performance)
   - **Timeout(ms)**: Recommended 500-10000 (adjust based on network)
   - **Auto Skip**: Check to skip IPs with existing successful addresses
4. Click the **「Start」** button to begin scanning
5. Wait for scanning to complete, monitor progress and success/failure statistics

### Step 4: Save Scan Results

1. After scanning completes, click the **「Save」** button
2. Choose save location and filename (recommended `.mc` format)
3. Scan results will be saved in importable format

### Step 5: Import Scan Results

1. Switch to the **「Settings」** tab
2. In the **「Import File」** section, click **「Select File」** button
3. Select the scan result file saved earlier
4. Click the **「Import」** button
5. After successful import, the number of imported records will be displayed

### Step 6: Channel Edit

1. Switch to the **「Channel Edit」** tab
2. View the imported channel list
3. You can edit the following fields:
   - **Channel ID**: Channel ID from EPG guide
   - **Channel**: Channel name
   - **Group**: Channel group (e.g., CCTV, Satellite)
   - **City**: Channel city
   - **Description**: Channel description
   - **Notes**: Additional notes
   - **LOGO**: Logo filename
4. Press Enter or click elsewhere to save automatically

### Step 7: Repeat Scanning

1. Return to the **「Settings」** tab
2. Modify multicast address or port configuration (if scanning other address ranges)
3. Repeat **Steps 2** to **6**

## 📺 Other Features

### Channel Browse

Switch to the **「Channel Browse」** tab to:
- View all channels and signal source information
- Click table headers to sort
- Paginated browsing (50 records per page)

### Channel Check

Switch to the **「Channel Check」** tab to:
- Detect signal source quality
- View video preview
- Compare detected values with database values
- Submit check results for update

### Export Features

In the **「Settings」** tab's **「Export File」** section, you can export to:
- **M3U**: Standard playlist format
- **TXT**: Text format playlist
- **XLS**: Excel spreadsheet format
- **CSV**: Comma-separated values format

### Import Other Formats

Besides scan results (`.mc` format), you can also import:
- **.m3u**: Standard M3U playlist
- **.txt**: TXT playlist

## ⚙️ Configuration Guide

### Address Template Examples

| Template | Expanded Result |
|----------|-----------------|
| `239.49.0.1` | 239.49.0.1 |
| `239.49.{0#1#2}` | 239.49.0, 239.49.1, 239.49.2 |
| `239.49.{[0-2]}` | 239.49.0, 239.49.1, 239.49.2 |
| `239.49.{0#1}.[1-10]` | 239.49.0.1-10, 239.49.1.1-10 |
| `{6000#8000}` | 6000, 8000 |
| `{[6000-6010]}` | 6000, 6001, ..., 6010 |

### Scan Parameter Recommendations

| Parameter | Recommended | Description |
|-----------|-------------|-------------|
| Threads | 8-16 | Based on server max connections |
| Timeout(ms) | 500-10000 | LAN: 500-1000, WAN: 10000 |
| Auto Skip | Enabled | Improves scanning efficiency |
| Fine Scan | Disabled | Enable only for precise probing |

## 🔧 Troubleshooting

### High Scan Failure Rate

- Check network connection
- Increase timeout
- Reduce thread count
- Verify address template correctness

### Import Failed

- Check file format correctness
- Confirm file encoding is UTF-8
- View error message

### No Video Preview

- Check if FFmpeg is properly installed
- Confirm streaming address is accessible
- Check if server is online

## 📞 Getting Help

For more help, click the **「Help」** button at the bottom of the software to view detailed feature descriptions.

---

**Version**: TVMC v2.2.0  
**Updated**: 2026-07-15
