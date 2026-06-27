#include "web_server.h"
#include "../app_state.h"

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// Web server instance private to this module
static WebServer server(80);

// ----- HTTP handlers -----
// static void handleRoot() {
//   const char* page =
//     R"HTML(<!doctype html>
//     <html lang="en">
//     <head>
//       <meta charset="utf-8"/>
//       <meta name="viewport" content="width=device-width, initial-scale=1"/>
//       <title>IAQM Dashboard</title>
//       <style>
//         :root{
//           --bg:#0b1220;
//           --panel:#111a2e;
//           --panel2:#0f172a;
//           --text:#e5e7eb;
//           --muted:#94a3b8;
//           --border:rgba(148,163,184,.18);

//           --good:#22c55e;
//           --warn:#f59e0b;
//           --bad:#ef4444;

//           --shadow: 0 10px 30px rgba(0,0,0,.35);
//           --radius: 16px;
//         }
//         *{box-sizing:border-box}
//         body{
//           margin:0;
//           font-family: ui-sans-serif, system-ui, -apple-system, Segoe UI, Roboto, Arial, sans-serif;
//           background: radial-gradient(1200px 600px at 20% -10%, rgba(59,130,246,.25), transparent 60%),
//                       radial-gradient(900px 500px at 100% 10%, rgba(34,197,94,.18), transparent 55%),
//                       var(--bg);
//           color:var(--text);
//           min-height:100vh;
//         }
//         .wrap{
//           max-width: 1100px;
//           margin: 0 auto;
//           padding: 18px 16px 28px 16px;
//         }
//         header{
//           display:flex;
//           align-items:flex-end;
//           justify-content:space-between;
//           gap: 12px;
//           margin-bottom: 14px;
//           flex-wrap: wrap;
//         }
//         .title{ display:flex; flex-direction:column; gap:4px; }
//         h1{ margin:0; font-size: 1.25rem; letter-spacing: .2px; font-weight: 650; }
//         .subtitle{ color:var(--muted); font-size: .95rem; }

//         .meta{
//           text-align:right;
//           color:var(--muted);
//           font-size: .9rem;
//           line-height: 1.35;
//           display:flex;
//           flex-direction:column;
//           gap: 6px;
//           align-items:flex-end;
//         }
//         .metaRow{ display:flex; gap:10px; align-items:center; justify-content:flex-end; flex-wrap:wrap; }

//         .dot{
//           display:inline-block;
//           width:10px;height:10px;
//           border-radius:50%;
//           margin-right:6px;
//           vertical-align:middle;
//           background: var(--muted);
//           box-shadow: 0 0 0 3px rgba(148,163,184,.12);
//         }
//         .dot.ok{ background: var(--good); box-shadow: 0 0 0 3px rgba(34,197,94,.15); }
//         .dot.bad{ background: var(--bad);  box-shadow: 0 0 0 3px rgba(239,68,68,.15); }

//         .banner{
//           border:1px solid var(--border);
//           background: linear-gradient(180deg, rgba(255,255,255,.04), rgba(255,255,255,.02));
//           border-radius: var(--radius);
//           box-shadow: var(--shadow);
//           padding: 14px 14px;
//           display:flex;
//           align-items:center;
//           justify-content:space-between;
//           gap: 12px;
//           margin: 12px 0 16px 0;
//           flex-wrap: wrap;
//         }
//         .banner .left{ display:flex; flex-direction:column; gap: 4px; }
//         .banner .label{
//           font-size:.85rem; color: var(--muted);
//           letter-spacing:.4px; text-transform: uppercase;
//         }
//         .banner .status{ font-size: 1.1rem; font-weight: 700; }
//         .pill{
//           display:inline-flex; align-items:center; gap: 8px;
//           padding: 8px 10px; border-radius: 999px;
//           border: 1px solid var(--border);
//           background: rgba(255,255,255,.03);
//           color: var(--text);
//           font-size: .9rem;
//           white-space: nowrap;
//         }
//         .pill.good{ border-color: rgba(34,197,94,.35); }
//         .pill.warn{ border-color: rgba(245,158,11,.35); }
//         .pill.bad { border-color: rgba(239,68,68,.35); }

//         .grid{
//           display:grid;
//           grid-template-columns: repeat(12, 1fr);
//           gap: 12px;
//         }

//         .card{
//           grid-column: span 6;
//           border:1px solid var(--border);
//           border-radius: var(--radius);
//           background: linear-gradient(180deg, rgba(255,255,255,.04), rgba(255,255,255,.015));
//           box-shadow: var(--shadow);
//           padding: 14px;
//           position:relative;
//           overflow:hidden;
//         }
//         .card::before{
//           content:"";
//           position:absolute;
//           inset:-80px -80px auto auto;
//           width:160px;height:160px;
//           background: radial-gradient(circle at 30% 30%, rgba(59,130,246,.20), transparent 60%);
//           transform: rotate(10deg);
//           pointer-events:none;
//         }

//         .cardHeader{
//           display:flex;
//           justify-content:space-between;
//           align-items:flex-start;
//           gap: 10px;
//           margin-bottom: 10px;
//         }

//         .k{
//           color: var(--muted);
//           font-size: .85rem;
//           letter-spacing: .25px;
//           text-transform: uppercase;
//         }
//         .v{
//           font-size: 2.0rem;
//           font-weight: 750;
//           line-height: 1.05;
//           letter-spacing: .2px;
//         }
//         .u{
//           color: var(--muted);
//           font-size: .9rem;
//           margin-left: 6px;
//           font-weight: 500;
//         }

//         .hint{
//           margin-top: 8px;
//           color: var(--muted);
//           font-size: .9rem;
//           display:flex;
//           justify-content:space-between;
//           gap: 12px;
//           flex-wrap: wrap;
//         }

//         .tag{
//           display:inline-flex;
//           align-items:center;
//           gap: 6px;
//           padding: 6px 8px;
//           border-radius: 999px;
//           border: 1px solid var(--border);
//           background: rgba(255,255,255,.03);
//           font-size: .82rem;
//           color: var(--text);
//           white-space: nowrap;
//         }
//         .tag.good{ border-color: rgba(34,197,94,.35); }
//         .tag.warn{ border-color: rgba(245,158,11,.35); }
//         .tag.bad { border-color: rgba(239,68,68,.35); }
//         .tag.neutral{ color: var(--muted); }

//         .card.good{ outline: 1px solid rgba(34,197,94,.30); }
//         .card.warn{ outline: 1px solid rgba(245,158,11,.30); }
//         .card.bad { outline: 1px solid rgba(239,68,68,.30); }

//         .wide{ grid-column: span 12; }
//         .span4{ grid-column: span 4; }

//         .footer{
//           margin-top: 14px;
//           color: var(--muted);
//           font-size: .9rem;
//           display:flex;
//           align-items:center;
//           justify-content:space-between;
//           gap: 12px;
//           flex-wrap: wrap;
//         }

//         .btn{
//           display:inline-flex;
//           align-items:center;
//           gap: 8px;
//           padding: 10px 12px;
//           border-radius: 12px;
//           border: 1px solid var(--border);
//           background: rgba(255,255,255,.03);
//           color: var(--text);
//           text-decoration:none;
//           font-weight: 650;
//           cursor:pointer;
//           user-select:none;
//         }
//         .btn:active{ transform: translateY(1px); }
//         .btn.small{ padding: 8px 10px; border-radius: 10px; font-size: .9rem; }
//         .btn.ghost{ background: transparent; }
//         .btn:disabled{
//           opacity: .5;
//           cursor:not-allowed;
//         }

//         .toast{
//           position: fixed;
//           left: 50%;
//           bottom: 18px;
//           transform: translateX(-50%);
//           background: rgba(17,26,46,.92);
//           border: 1px solid var(--border);
//           color: var(--text);
//           padding: 10px 12px;
//           border-radius: 12px;
//           box-shadow: var(--shadow);
//           font-size: .92rem;
//           opacity: 0;
//           pointer-events:none;
//           transition: opacity .18s ease;
//           z-index: 999;
//           max-width: calc(100vw - 40px);
//         }
//         .toast.show{ opacity: 1; }

//         .stale{
//           opacity: .55;
//           filter: grayscale(.35);
//         }

//         /* Charts */
//         .chartWrap{
//           margin-top: 12px;
//           border-top: 1px solid rgba(148,163,184,.14);
//           padding-top: 10px;
//         }
//         canvas.chart{
//           width: 100%;
//           height: 120px;
//           display:block;
//           border-radius: 12px;
//           background: rgba(255,255,255,.02);
//           border: 1px solid rgba(148,163,184,.12);
//         }
//         .chartMeta{
//           display:flex;
//           justify-content:space-between;
//           gap:12px;
//           margin-top: 8px;
//           color: var(--muted);
//           font-size: .85rem;
//           flex-wrap: wrap;
//         }

//         @media (max-width: 720px){
//           .card{ grid-column: span 12; }
//           .meta{ align-items:flex-start; text-align:left; }
//           .metaRow{ justify-content:flex-start; }
//         }
//       </style>
//     </head>
//     <body>
//       <div class="wrap">
//         <header>
//           <div class="title">
//             <h1>IAQM Dashboard</h1>
//             <div class="subtitle">Live readings from ESP32-S3 • Updated automatically</div>
//           </div>

//           <div class="meta">
//             <div class="metaRow">
//               <div><span id="connDot" class="dot"></span><span id="connText">Connecting...</span></div>
//               <button id="btnCopy" class="btn small ghost" type="button">Copy link</button>
//               <button id="btnUnits" class="btn small ghost" type="button">°C</button>
//             </div>
//             <div>Last update: <span id="lastUpdate">—</span></div>
//           </div>
//         </header>

//         <div id="banner" class="banner">
//           <div class="left">
//             <div class="label">Overall Air Quality</div>
//             <div id="aqStatus" class="status">Loading…</div>
//             <div id="aqAdvice" class="subtitle">Fetching sensor data…</div>
//           </div>
//           <div id="aqPill" class="pill">—</div>
//         </div>

//         <div id="grid" class="grid">
//           <!-- CO2 -->
//           <div id="cardCo2" class="card">
//             <div class="cardHeader">
//               <div>
//                 <div class="k">CO₂</div>
//                 <div><span id="co2" class="v">—</span><span class="u">ppm</span></div>
//               </div>
//               <div id="co2Tag" class="tag neutral">—</div>
//             </div>
//             <div class="hint">
//               <span>Ventilation indicator</span>
//               <span id="s88ok" class="tag neutral">S88: —</span>
//             </div>
//             <div class="chartWrap">
//               <canvas id="chCo2" class="chart" width="900" height="240"></canvas>
//               <div class="chartMeta">
//                 <span>History (rolling)</span>
//                 <span id="co2Range">—</span>
//               </div>
//             </div>
//           </div>

//           <!-- Temperature -->
//           <div id="cardTemp" class="card">
//             <div class="cardHeader">
//               <div>
//                 <div class="k">Temperature</div>
//                 <div><span id="t" class="v">—</span><span id="tUnit" class="u">°C</span></div>
//               </div>
//               <div id="tTag" class="tag neutral">—</div>
//             </div>
//             <div class="hint">
//               <span>Comfort</span>
//               <span id="shtok" class="tag neutral">SHT45: —</span>
//             </div>
//             <div class="chartWrap">
//               <canvas id="chT" class="chart" width="900" height="240"></canvas>
//               <div class="chartMeta">
//                 <span>History (rolling)</span>
//                 <span id="tRange">—</span>
//               </div>
//             </div>
//           </div>

//           <!-- Humidity -->
//           <div id="cardRh" class="card">
//             <div class="cardHeader">
//               <div>
//                 <div class="k">Humidity</div>
//                 <div><span id="rh" class="v">—</span><span class="u">%</span></div>
//               </div>
//               <div id="rhTag" class="tag neutral">—</div>
//             </div>
//             <div class="hint">
//               <span>Comfort</span>
//               <span class="tag neutral">Target 30–60%</span>
//             </div>
//             <div class="chartWrap">
//               <canvas id="chRh" class="chart" width="900" height="240"></canvas>
//               <div class="chartMeta">
//                 <span>History (rolling)</span>
//                 <span id="rhRange">—</span>
//               </div>
//             </div>
//           </div>

//           <!-- Ambient Light -->
//           <div id="cardLux" class="card">
//             <div class="cardHeader">
//               <div>
//                 <div class="k">Ambient Light</div>
//                 <div><span id="lux" class="v">—</span><span class="u">lux</span></div>
//               </div>
//               <div id="luxTag" class="tag neutral">—</div>
//             </div>
//             <div class="hint">
//               <span id="vemlok" class="tag neutral">VEML7700: —</span>
//               <span class="tag neutral">ALS: <span id="als">—</span></span>
//             </div>
//           </div>

//           <!-- Export Stub -->
//           <div class="card wide" id="cardExport">
//             <div class="cardHeader">
//               <div>
//                 <div class="k">Export</div>
//                 <div class="subtitle">Local SD export will appear here once logging is enabled.</div>
//               </div>
//               <div class="tag neutral">Coming soon</div>
//             </div>
//             <div class="hint">
//               <div class="tag neutral">Planned: Download CSV by day • /download?date=YYYY-MM-DD</div>
//               <div style="display:flex; gap:10px; flex-wrap:wrap;">
//                 <button class="btn" type="button" disabled>Download last 24h</button>
//                 <button class="btn ghost" type="button" disabled>List files</button>
//               </div>
//             </div>
//           </div>

//           <!-- System -->
//           <div class="card wide">
//             <div class="cardHeader">
//               <div>
//                 <div class="k">System</div>
//                 <div style="display:flex;align-items:baseline;gap:10px;">
//                   <div><span id="ms" class="v" style="font-size:1.6rem;">—</span><span class="u">ms uptime</span></div>
//                 </div>
//               </div>
//               <div class="tag neutral">Endpoints: /json • /metrics</div>
//             </div>
//             <div class="hint">
//               <span class="tag neutral">Tip: Use <b>iaqm.local</b> on this network</span>
//               <a class="btn" href="/metrics" target="_blank" rel="noopener">Open /metrics</a>
//             </div>
//           </div>
//         </div>

//         <div class="footer">
//           <div>Data source: <span class="tag neutral">/json</span> polled every 2 seconds</div>
//           <a class="btn" href="/json" target="_blank" rel="noopener">View raw JSON</a>
//         </div>
//       </div>

//       <div id="toast" class="toast">—</div>

//     <script>
//       const $ = (id) => document.getElementById(id);

//       // -----------------------------
//       // Connection / stale handling
//       // -----------------------------
//       let lastOkMs = 0;
//       const STALE_AFTER_MS = 8000;

//       function setConnection(ok){
//         $('connDot').className = 'dot ' + (ok ? 'ok' : 'bad');
//         $('connText').textContent = ok ? 'Connected' : 'Disconnected';
//       }
//       function setStale(stale){
//         $('grid').classList.toggle('stale', stale);
//         $('banner').classList.toggle('stale', stale);
//       }

//       // -----------------------------
//       // Units toggle (°C/°F)
//       // -----------------------------
//       let useF = (localStorage.getItem('iaqm_units') === 'F');

//       function setUnitsButton(){
//         $('btnUnits').textContent = useF ? '°F' : '°C';
//         $('tUnit').textContent = useF ? '°F' : '°C';
//       }
//       function cToF(c){ return (c * 9/5) + 32; }

//       $('btnUnits').addEventListener('click', () => {
//         useF = !useF;
//         localStorage.setItem('iaqm_units', useF ? 'F' : 'C');
//         setUnitsButton();
//         // Re-render charts (units change affects temperature chart scale/labels)
//         redrawAllCharts();
//       });
//       setUnitsButton();

//       // -----------------------------
//       // Copy link button
//       // -----------------------------
//       function showToast(msg){
//         const t = $('toast');
//         t.textContent = msg;
//         t.classList.add('show');
//         setTimeout(()=>t.classList.remove('show'), 1800);
//       }

//       $('btnCopy').addEventListener('click', async () => {
//         const url = 'http://iaqm.local/';
//         try{
//           if (navigator.clipboard && window.isSecureContext) {
//             await navigator.clipboard.writeText(url);
//             showToast('Copied: ' + url);
//           } else {
//             // Fallback: prompt
//             window.prompt('Copy this link:', url);
//           }
//         } catch(e){
//           window.prompt('Copy this link:', url);
//         }
//       });

//       // -----------------------------
//       // Threshold helpers (tunable)
//       // -----------------------------
//       function bandCo2(ppm){
//         if (!isFinite(ppm)) return {cls:'neutral', label:'—'};
//         if (ppm < 800)  return {cls:'good', label:'Good'};
//         if (ppm < 1200) return {cls:'warn', label:'Moderate'};
//         return {cls:'bad', label:'High'};
//       }

//       function bandHumidity(rh){
//         if (!isFinite(rh)) return {cls:'neutral', label:'—'};
//         if (rh < 30) return {cls:'warn', label:'Dry'};
//         if (rh <= 60) return {cls:'good', label:'Good'};
//         return {cls:'warn', label:'Humid'};
//       }

//       function bandTempC(tC){
//         if (!isFinite(tC)) return {cls:'neutral', label:'—'};
//         // Comfort in C; if user chooses F, we still evaluate comfort in C
//         if (tC < 18) return {cls:'warn', label:'Cool'};
//         if (tC <= 26) return {cls:'good', label:'Comfort'};
//         return {cls:'warn', label:'Warm'};
//       }

//       function bandLux(lux){
//         if (!isFinite(lux)) return {cls:'neutral', label:'—'};
//         if (lux < 50) return {cls:'neutral', label:'Dim'};
//         if (lux < 300) return {cls:'neutral', label:'Indoor'};
//         return {cls:'neutral', label:'Bright'};
//       }

//       function setTag(el, band){
//         el.className = 'tag ' + (band.cls || 'neutral');
//         el.textContent = band.label || '—';
//       }

//       function setCardBand(card, band){
//         card.classList.remove('good','warn','bad');
//         if (band.cls === 'good') card.classList.add('good');
//         else if (band.cls === 'warn') card.classList.add('warn');
//         else if (band.cls === 'bad') card.classList.add('bad');
//       }

//       function overallAQ(co2Band, shtOk, s88Ok){
//         if (!shtOk || !s88Ok) return {cls:'warn', title:'Moderate', advice:'One or more sensors report unhealthy status.'};
//         if (co2Band.cls === 'bad')  return {cls:'bad',  title:'Poor', advice:'CO₂ is high. Consider ventilation.'};
//         if (co2Band.cls === 'warn') return {cls:'warn', title:'Moderate', advice:'CO₂ is elevated. Monitor ventilation.'};
//         return {cls:'good', title:'Good', advice:'Conditions look good.'};
//       }

//       function setBanner(b){
//         $('aqStatus').textContent = b.title;
//         $('aqAdvice').textContent = b.advice;

//         const pill = $('aqPill');
//         pill.className = 'pill ' + b.cls;
//         pill.innerHTML = '<span class="dot ' + (b.cls==='good'?'ok':(b.cls==='bad'?'bad':'')) + '"></span>' + b.title;
//       }

//       // -----------------------------
//       // Formatting
//       // -----------------------------
//       function fmt2(x){ return (isFinite(x) ? x.toFixed(2) : '—'); }
//       function fmt0(x){ return (isFinite(x) ? Math.round(x).toString() : '—'); }

//       // -----------------------------
//       // Lightweight charts (canvas)
//       // -----------------------------
//       const HISTORY_MAX = 300; // 300 points at 2s => ~10 minutes
//       const hist = {
//         co2: [],
//         tC: [],
//         rh: []
//       };

//       function pushHist(arr, v){
//         if (!isFinite(v)) return;
//         arr.push(v);
//         if (arr.length > HISTORY_MAX) arr.shift();
//       }

//       function minmax(arr){
//         if (!arr.length) return {min:0,max:1};
//         let mn = arr[0], mx = arr[0];
//         for (let i=1;i<arr.length;i++){
//           const v = arr[i];
//           if (v < mn) mn = v;
//           if (v > mx) mx = v;
//         }
//         if (mn === mx){
//           // expand a bit so the line is visible
//           mn -= 1; mx += 1;
//         }
//         return {min:mn, max:mx};
//       }

//       function drawChart(canvas, arr, opts){
//         const ctx = canvas.getContext('2d');
//         const w = canvas.width, h = canvas.height;

//         // Clear
//         ctx.clearRect(0,0,w,h);

//         // Background + subtle grid
//         ctx.fillStyle = 'rgba(255,255,255,0.00)';
//         ctx.fillRect(0,0,w,h);

//         const padL = 34, padR = 10, padT = 10, padB = 22;
//         const gw = w - padL - padR;
//         const gh = h - padT - padB;

//         // Grid lines
//         ctx.strokeStyle = 'rgba(148,163,184,0.16)';
//         ctx.lineWidth = 1;
//         ctx.beginPath();
//         for (let i=0;i<=4;i++){
//           const y = padT + (gh * i/4);
//           ctx.moveTo(padL, y);
//           ctx.lineTo(padL+gw, y);
//         }
//         ctx.stroke();

//         // If not enough points, show "collecting"
//         if (arr.length < 3){
//           ctx.fillStyle = 'rgba(148,163,184,0.70)';
//           ctx.font = '14px ui-sans-serif, system-ui';
//           ctx.fillText('Collecting history…', padL+10, padT+24);
//           return;
//         }

//         // Determine min/max
//         const mm = minmax(arr);
//         let mn = mm.min, mx = mm.max;

//         // Optional fixed bounds
//         if (opts && isFinite(opts.min)) mn = opts.min;
//         if (opts && isFinite(opts.max)) mx = opts.max;
//         if (mn === mx){ mn -= 1; mx += 1; }

//         // Axis labels (min/max)
//         ctx.fillStyle = 'rgba(148,163,184,0.85)';
//         ctx.font = '12px ui-sans-serif, system-ui';
//         ctx.fillText(mn.toFixed(opts?.decimals ?? 0), 4, padT + gh);
//         ctx.fillText(mx.toFixed(opts?.decimals ?? 0), 4, padT + 12);

//         // Line
//         ctx.strokeStyle = 'rgba(59,130,246,0.95)';
//         ctx.lineWidth = 2;
//         ctx.beginPath();

//         for (let i=0;i<arr.length;i++){
//           const x = padL + (gw * i/(arr.length-1));
//           const v = arr[i];
//           const yNorm = (v - mn) / (mx - mn);
//           const y = padT + gh - (gh * yNorm);
//           if (i === 0) ctx.moveTo(x,y);
//           else ctx.lineTo(x,y);
//         }
//         ctx.stroke();

//         // Latest point marker
//         const last = arr[arr.length-1];
//         const x = padL + gw;
//         const yNorm = (last - mn) / (mx - mn);
//         const y = padT + gh - (gh * yNorm);

//         ctx.fillStyle = 'rgba(59,130,246,1.0)';
//         ctx.beginPath();
//         ctx.arc(x, y, 3.5, 0, Math.PI*2);
//         ctx.fill();
//       }

//       function redrawAllCharts(){
//         // CO2 chart: autoscale, integer labels
//         drawChart($('chCo2'), hist.co2, {decimals:0});
//         // RH chart: fixed 0-100 is nice for interpretation
//         drawChart($('chRh'), hist.rh, {min:0, max:100, decimals:0});
//         // Temp chart: depends on unit selection
//         if (useF){
//           const arrF = hist.tC.map(cToF);
//           drawChart($('chT'), arrF, {decimals:1});
//         } else {
//           drawChart($('chT'), hist.tC, {decimals:1});
//         }

//         // Update ranges
//         if (hist.co2.length){
//           const mm = minmax(hist.co2);
//           $('co2Range').textContent = 'Range: ' + Math.round(mm.min) + '–' + Math.round(mm.max) + ' ppm';
//         } else $('co2Range').textContent = '—';

//         if (hist.rh.length){
//           const mm = minmax(hist.rh);
//           $('rhRange').textContent = 'Range: ' + mm.min.toFixed(1) + '–' + mm.max.toFixed(1) + ' %';
//         } else $('rhRange').textContent = '—';

//         if (hist.tC.length){
//           const mm = minmax(hist.tC);
//           if (useF){
//             $('tRange').textContent = 'Range: ' + cToF(mm.min).toFixed(1) + '–' + cToF(mm.max).toFixed(1) + ' °F';
//           } else {
//             $('tRange').textContent = 'Range: ' + mm.min.toFixed(1) + '–' + mm.max.toFixed(1) + ' °C';
//           }
//         } else $('tRange').textContent = '—';
//       }

//       // -----------------------------
//       // Main polling loop
//       // -----------------------------
//       async function tick(){
//         try{
//           const r = await fetch('/json', {cache:'no-store'});
//           const j = await r.json();

//           lastOkMs = Date.now();
//           setConnection(true);
//           $('lastUpdate').textContent = new Date().toLocaleTimeString();

//           const tC  = Number(j.temperature_C);
//           const rh  = Number(j.humidity_pct);
//           const co2 = Number(j.co2_ppm);
//           const lux = Number(j.lux);
//           const als = Number(j.als);
//           const ms  = Number(j.uptime_ms);

//           const shtOk  = !!j.sht45_ok;
//           const vemlOk = !!j.veml7700_ok;
//           const s88Ok  = (j.s88_ok === undefined) ? true : !!j.s88_ok;

//           // Display values
//           const tDisp = useF ? cToF(tC) : tC;
//           $('t').textContent   = fmt2(tDisp);
//           $('rh').textContent  = fmt2(rh);
//           $('lux').textContent = fmt2(lux);
//           $('als').textContent = isFinite(als) ? als : '—';
//           $('ms').textContent  = isFinite(ms) ? ms : '—';
//           $('co2').textContent = isFinite(co2) ? Math.round(co2) : '—';

//           // Health tags
//           $('shtok').textContent = 'SHT45: ' + (shtOk ? 'OK' : 'ERR');
//           $('shtok').className = 'tag ' + (shtOk ? 'good' : 'bad');

//           $('vemlok').textContent = 'VEML7700: ' + (vemlOk ? 'OK' : 'ERR');
//           $('vemlok').className = 'tag ' + (vemlOk ? 'good' : 'bad');

//           $('s88ok').textContent = 'S88: ' + (s88Ok ? 'OK' : 'ERR');
//           $('s88ok').className = 'tag ' + (s88Ok ? 'good' : 'bad');

//           // Bands (semantic)
//           const bCo2 = bandCo2(co2);
//           const bRh  = bandHumidity(rh);
//           const bT   = bandTempC(tC);
//           const bLux = bandLux(lux);

//           setTag($('co2Tag'), bCo2);
//           setTag($('rhTag'), bRh);
//           setTag($('tTag'), bT);
//           setTag($('luxTag'), bLux);

//           setCardBand($('cardCo2'), bCo2);
//           setCardBand($('cardRh'), bRh);
//           setCardBand($('cardTemp'), bT);

//           // Lux is informational; keep neutral styling by default

//           // Overall banner
//           const overall = overallAQ(bCo2, shtOk, s88Ok);
//           setBanner(overall);

//           // History + charts
//           pushHist(hist.co2, co2);
//           pushHist(hist.rh, rh);
//           pushHist(hist.tC, tC);
//           redrawAllCharts();

//           setStale(false);
//         } catch(e){
//           setConnection(false);
//           const stale = (Date.now() - lastOkMs) > STALE_AFTER_MS;
//           setStale(stale);
//         }
//       }

//       // Start
//       tick();
//       setInterval(tick, 2000);

//       // Stale watchdog
//       setInterval(() => {
//         const stale = (Date.now() - lastOkMs) > STALE_AFTER_MS;
//         setStale(stale);
//       }, 1000);

//       // Redraw charts on resize (keeps crispness for some browsers)
//       window.addEventListener('resize', () => {
//         // Canvas uses fixed internal resolution; CSS scales it.
//         // We keep it simple: just redraw with current history.
//         redrawAllCharts();
//       });

//     </script>
//     </body>
//     </html>)HTML";

//   server.send(200, "text/html; charset=utf-8", page);
// }

static void handleRoot() {
  const char* page =
  R"HTML(<!doctype html>
  <html lang="en">
  <head>
    <meta charset="utf-8"/>
    <meta name="viewport" content="width=device-width, initial-scale=1"/>
    <title>IAQM Dashboard</title>
    <style>
      :root{
        --bg:#0b1220;
        --panel:#111a2e;
        --panel2:#0f172a;
        --text:#e5e7eb;
        --muted:#94a3b8;
        --border:rgba(148,163,184,.18);

        --good:#22c55e;
        --warn:#f59e0b;
        --bad:#ef4444;

        --shadow: 0 10px 30px rgba(0,0,0,.35);
        --radius: 16px;
      }
      *{box-sizing:border-box}
      body{
        margin:0;
        font-family: ui-sans-serif, system-ui, -apple-system, Segoe UI, Roboto, Arial, sans-serif;
        background: radial-gradient(1200px 600px at 20% -10%, rgba(59,130,246,.25), transparent 60%),
                    radial-gradient(900px 500px at 100% 10%, rgba(34,197,94,.18), transparent 55%),
                    var(--bg);
        color:var(--text);
        min-height:100vh;
      }
      .wrap{
        max-width: 1180px;
        margin: 0 auto;
        padding: 18px 16px 28px 16px;
      }
      header{
        display:flex;
        align-items:flex-end;
        justify-content:space-between;
        gap: 12px;
        margin-bottom: 14px;
        flex-wrap: wrap;
      }
      .title{ display:flex; flex-direction:column; gap:4px; }
      h1{ margin:0; font-size: 1.25rem; letter-spacing: .2px; font-weight: 650; }
      .subtitle{ color:var(--muted); font-size: .95rem; }

      .meta{
        text-align:right;
        color:var(--muted);
        font-size: .9rem;
        line-height: 1.35;
        display:flex;
        flex-direction:column;
        gap: 6px;
        align-items:flex-end;
      }
      .metaRow{ display:flex; gap:10px; align-items:center; justify-content:flex-end; flex-wrap:wrap; }

      .dot{
        display:inline-block;
        width:10px;height:10px;
        border-radius:50%;
        margin-right:6px;
        vertical-align:middle;
        background: var(--muted);
        box-shadow: 0 0 0 3px rgba(148,163,184,.12);
      }
      .dot.ok{ background: var(--good); box-shadow: 0 0 0 3px rgba(34,197,94,.15); }
      .dot.bad{ background: var(--bad);  box-shadow: 0 0 0 3px rgba(239,68,68,.15); }

      .banner{
        border:1px solid var(--border);
        background: linear-gradient(180deg, rgba(255,255,255,.04), rgba(255,255,255,.02));
        border-radius: var(--radius);
        box-shadow: var(--shadow);
        padding: 14px 14px;
        display:flex;
        align-items:center;
        justify-content:space-between;
        gap: 12px;
        margin: 12px 0 16px 0;
        flex-wrap: wrap;
      }
      .banner .left{ display:flex; flex-direction:column; gap: 4px; }
      .banner .label{
        font-size:.85rem; color: var(--muted);
        letter-spacing:.4px; text-transform: uppercase;
      }
      .banner .status{ font-size: 1.1rem; font-weight: 700; }

      .pill{
        display:inline-flex; align-items:center; gap: 8px;
        padding: 8px 10px; border-radius: 999px;
        border: 1px solid var(--border);
        background: rgba(255,255,255,.03);
        color: var(--text);
        font-size: .9rem;
        white-space: nowrap;
      }
      .pill.good{ border-color: rgba(34,197,94,.35); }
      .pill.warn{ border-color: rgba(245,158,11,.35); }
      .pill.bad { border-color: rgba(239,68,68,.35); }

      .grid{
        display:grid;
        grid-template-columns: repeat(12, 1fr);
        gap: 12px;
      }

      .card{
        grid-column: span 6;
        border:1px solid var(--border);
        border-radius: var(--radius);
        background: linear-gradient(180deg, rgba(255,255,255,.04), rgba(255,255,255,.015));
        box-shadow: var(--shadow);
        padding: 14px;
        position:relative;
        overflow:hidden;
      }
      .card::before{
        content:"";
        position:absolute;
        inset:-80px -80px auto auto;
        width:160px;height:160px;
        background: radial-gradient(circle at 30% 30%, rgba(59,130,246,.20), transparent 60%);
        transform: rotate(10deg);
        pointer-events:none;
      }

      .cardHeader{
        display:flex;
        justify-content:space-between;
        align-items:flex-start;
        gap: 10px;
        margin-bottom: 10px;
      }

      .k{
        color: var(--muted);
        font-size: .85rem;
        letter-spacing: .25px;
        text-transform: uppercase;
      }
      .v{
        font-size: 2.0rem;
        font-weight: 750;
        line-height: 1.05;
        letter-spacing: .2px;
      }
      .u{
        color: var(--muted);
        font-size: .9rem;
        margin-left: 6px;
        font-weight: 500;
      }

      .hint{
        margin-top: 8px;
        color: var(--muted);
        font-size: .9rem;
        display:flex;
        justify-content:space-between;
        gap: 12px;
        flex-wrap: wrap;
      }

      .tag{
        display:inline-flex;
        align-items:center;
        gap: 6px;
        padding: 6px 8px;
        border-radius: 999px;
        border: 1px solid var(--border);
        background: rgba(255,255,255,.03);
        font-size: .82rem;
        color: var(--text);
        white-space: nowrap;
      }
      .tag.good{ border-color: rgba(34,197,94,.35); }
      .tag.warn{ border-color: rgba(245,158,11,.35); }
      .tag.bad { border-color: rgba(239,68,68,.35); }
      .tag.neutral{ color: var(--muted); }

      .card.good{ outline: 1px solid rgba(34,197,94,.30); }
      .card.warn{ outline: 1px solid rgba(245,158,11,.30); }
      .card.bad { outline: 1px solid rgba(239,68,68,.30); }

      .wide{ grid-column: span 12; }
      .span4{ grid-column: span 4; }

      .footer{
        margin-top: 14px;
        color: var(--muted);
        font-size: .9rem;
        display:flex;
        align-items:center;
        justify-content:space-between;
        gap: 12px;
        flex-wrap: wrap;
      }

      .btn{
        display:inline-flex;
        align-items:center;
        gap: 8px;
        padding: 10px 12px;
        border-radius: 12px;
        border: 1px solid var(--border);
        background: rgba(255,255,255,.03);
        color: var(--text);
        text-decoration:none;
        font-weight: 650;
        cursor:pointer;
        user-select:none;
      }
      .btn:active{ transform: translateY(1px); }
      .btn.small{ padding: 8px 10px; border-radius: 10px; font-size: .9rem; }
      .btn.ghost{ background: transparent; }
      .btn:disabled{ opacity: .5; cursor:not-allowed; }

      .toast{
        position: fixed;
        left: 50%;
        bottom: 18px;
        transform: translateX(-50%);
        background: rgba(17,26,46,.92);
        border: 1px solid var(--border);
        color: var(--text);
        padding: 10px 12px;
        border-radius: 12px;
        box-shadow: var(--shadow);
        font-size: .92rem;
        opacity: 0;
        pointer-events:none;
        transition: opacity .18s ease;
        z-index: 999;
        max-width: calc(100vw - 40px);
      }
      .toast.show{ opacity: 1; }

      .stale{ opacity: .55; filter: grayscale(.35); }

      /* Charts */
      .chartWrap{
        margin-top: 12px;
        border-top: 1px solid rgba(148,163,184,.14);
        padding-top: 10px;
      }
      canvas.chart{
        width: 100%;
        height: 120px;
        display:block;
        border-radius: 12px;
        background: rgba(255,255,255,.02);
        border: 1px solid rgba(148,163,184,.12);
      }
      .chartMeta{
        display:flex;
        justify-content:space-between;
        gap:12px;
        margin-top: 8px;
        color: var(--muted);
        font-size: .85rem;
        flex-wrap: wrap;
      }

      @media (max-width: 720px){
        .card{ grid-column: span 12; }
        .meta{ align-items:flex-start; text-align:left; }
        .metaRow{ justify-content:flex-start; }
      }
    </style>
  </head>
  <body>
    <div class="wrap">
      <header>
        <div class="title">
          <h1>IAQM Dashboard</h1>
          <div class="subtitle">Live readings from ESP32-S3 • Updated automatically</div>
        </div>

        <div class="meta">
          <div class="metaRow">
            <div><span id="connDot" class="dot"></span><span id="connText">Connecting...</span></div>
            <button id="btnCopy" class="btn small ghost" type="button">Copy link</button>
            <button id="btnUnits" class="btn small ghost" type="button">°C</button>
          </div>
          <div>Last update: <span id="lastUpdate">—</span></div>
        </div>
      </header>

      <div id="banner" class="banner">
        <div class="left">
          <div class="label">Overall Air Quality</div>
          <div id="aqStatus" class="status">Loading…</div>
          <div id="aqAdvice" class="subtitle">Fetching sensor data…</div>
        </div>
        <div id="aqPill" class="pill">—</div>
      </div>

      <div id="grid" class="grid">
        <!-- CO2 -->
        <div id="cardCo2" class="card">
          <div class="cardHeader">
            <div>
              <div class="k">CO₂</div>
              <div><span id="co2" class="v">—</span><span class="u">ppm</span></div>
            </div>
            <div id="co2Tag" class="tag neutral">—</div>
          </div>
          <div class="hint">
            <span>Ventilation indicator</span>
            <span id="s88ok" class="tag neutral">S88: —</span>
          </div>
        </div>

        <!-- PM2.5 + chart -->
        <div id="cardPm25" class="card">
          <div class="cardHeader">
            <div>
              <div class="k">PM2.5</div>
              <div><span id="pm25" class="v">—</span><span class="u">µg/m³</span></div>
            </div>
            <div id="pm25Tag" class="tag neutral">—</div>
          </div>
          <div class="hint">
            <span>Fine particles</span>
            <span id="sen55ok" class="tag neutral">SEN55: —</span>
          </div>
          <div class="chartWrap">
            <canvas id="chPm25" class="chart" width="900" height="240"></canvas>
            <div class="chartMeta">
              <span>History (rolling)</span>
              <span id="pm25Range">—</span>
            </div>
          </div>
        </div>

        <!-- PM10 -->
        <div id="cardPm10" class="card">
          <div class="cardHeader">
            <div>
              <div class="k">PM10</div>
              <div><span id="pm10" class="v">—</span><span class="u">µg/m³</span></div>
            </div>
            <div id="pm10Tag" class="tag neutral">—</div>
          </div>
          <div class="hint">
            <span>Coarse particles</span>
            <span class="tag neutral">From SEN55</span>
          </div>
        </div>

        <!-- VOC Index + chart -->
        <div id="cardVoc" class="card">
          <div class="cardHeader">
            <div>
              <div class="k">VOC Index</div>
              <div><span id="voc" class="v">—</span><span class="u">index</span></div>
            </div>
            <div id="vocTag" class="tag neutral">—</div>
          </div>
          <div class="hint">
            <span>Relative VOC level</span>
            <span class="tag neutral">Warm-up after boot</span>
          </div>
          <div class="chartWrap">
            <canvas id="chVoc" class="chart" width="900" height="240"></canvas>
            <div class="chartMeta">
              <span>History (rolling)</span>
              <span id="vocRange">—</span>
            </div>
          </div>
        </div>

        <!-- NOx Index -->
        <div id="cardNox" class="card">
          <div class="cardHeader">
            <div>
              <div class="k">NOx Index</div>
              <div><span id="nox" class="v">—</span><span class="u">index</span></div>
            </div>
            <div id="noxTag" class="tag neutral">—</div>
          </div>
          <div class="hint">
            <span>Relative NOx level</span>
            <span class="tag neutral">From SEN55</span>
          </div>
        </div>

        <!-- Temperature -->
        <div id="cardTemp" class="card">
          <div class="cardHeader">
            <div>
              <div class="k">Temperature</div>
              <div><span id="t" class="v">—</span><span id="tUnit" class="u">°C</span></div>
            </div>
            <div id="tTag" class="tag neutral">—</div>
          </div>
          <div class="hint">
            <span>From SEN55</span>
            <span id="shtok" class="tag neutral">SEN55 T/RH: —</span>
          </div>
        </div>

        <!-- Humidity -->
        <div id="cardRh" class="card">
          <div class="cardHeader">
            <div>
              <div class="k">Humidity</div>
              <div><span id="rh" class="v">—</span><span class="u">%</span></div>
            </div>
            <div id="rhTag" class="tag neutral">—</div>
          </div>
          <div class="hint">
            <span>From SEN55</span>
            <span class="tag neutral">Target 30–60%</span>
          </div>
        </div>

        <!-- Ambient Light -->
        <div id="cardLux" class="card">
          <div class="cardHeader">
            <div>
              <div class="k">Ambient Light</div>
              <div><span id="lux" class="v">—</span><span class="u">lux</span></div>
            </div>
            <div id="luxTag" class="tag neutral">—</div>
          </div>
          <div class="hint">
            <span id="vemlok" class="tag neutral">VEML7700: —</span>
            <span class="tag neutral">ALS: <span id="als">—</span></span>
          </div>
        </div>

        <!-- Export Stub -->
        <div class="card wide" id="cardExport">
          <div class="cardHeader">
            <div>
              <div class="k">Export</div>
              <div class="subtitle">SD logging will appear here once enabled (Adalogger FeatherWing).</div>
            </div>
            <div class="tag neutral">Coming soon</div>
          </div>
          <div class="hint">
            <div class="tag neutral">Planned: Download CSV by day • /download?date=YYYY-MM-DD</div>
            <div style="display:flex; gap:10px; flex-wrap:wrap;">
              <button class="btn" type="button" disabled>Download last 24h</button>
              <button class="btn ghost" type="button" disabled>List files</button>
            </div>
          </div>
        </div>

        <!-- System -->
        <div class="card wide">
          <div class="cardHeader">
            <div>
              <div class="k">System</div>
              <div style="display:flex;align-items:baseline;gap:10px;">
                <div><span id="ms" class="v" style="font-size:1.6rem;">—</span><span class="u">ms uptime</span></div>
              </div>
            </div>
            <div class="tag neutral">Endpoints: /json • /metrics</div>
          </div>
          <div class="hint">
            <span class="tag neutral">Tip: Use <b>iaqm.local</b> on this network</span>
            <a class="btn" href="/metrics" target="_blank" rel="noopener">Open /metrics</a>
          </div>
        </div>
      </div>

      <div class="footer">
        <div>Data source: <span class="tag neutral">/json</span> polled every 2 seconds</div>
        <a class="btn" href="/json" target="_blank" rel="noopener">View raw JSON</a>
      </div>
    </div>

    <div id="toast" class="toast">—</div>

  <script>
    const $ = (id) => document.getElementById(id);

    // -----------------------------
    // Connection / stale handling
    // -----------------------------
    let lastOkMs = 0;
    const STALE_AFTER_MS = 8000;

    function setConnection(ok){
      $('connDot').className = 'dot ' + (ok ? 'ok' : 'bad');
      $('connText').textContent = ok ? 'Connected' : 'Disconnected';
    }
    function setStale(stale){
      $('grid').classList.toggle('stale', stale);
      $('banner').classList.toggle('stale', stale);
    }

    // -----------------------------
    // Units toggle (°C/°F)
    // -----------------------------
    let useF = (localStorage.getItem('iaqm_units') === 'F');

    function setUnitsButton(){
      $('btnUnits').textContent = useF ? '°F' : '°C';
      $('tUnit').textContent = useF ? '°F' : '°C';
    }
    function cToF(c){ return (c * 9/5) + 32; }

    $('btnUnits').addEventListener('click', () => {
      useF = !useF;
      localStorage.setItem('iaqm_units', useF ? 'F' : 'C');
      setUnitsButton();
    });
    setUnitsButton();

    // -----------------------------
    // Copy link button
    // -----------------------------
    function showToast(msg){
      const t = $('toast');
      t.textContent = msg;
      t.classList.add('show');
      setTimeout(()=>t.classList.remove('show'), 1800);
    }

    $('btnCopy').addEventListener('click', async () => {
      const url = 'http://iaqm.local/';
      try{
        if (navigator.clipboard && window.isSecureContext) {
          await navigator.clipboard.writeText(url);
          showToast('Copied: ' + url);
        } else {
          window.prompt('Copy this link:', url);
        }
      } catch(e){
        window.prompt('Copy this link:', url);
      }
    });

    // -----------------------------
    // Threshold helpers (tunable)
    // -----------------------------
    // CO2 ppm thresholds
    function bandCo2(ppm){
      if (!isFinite(ppm)) return {cls:'neutral', label:'—'};
      if (ppm < 800)  return {cls:'good', label:'Good'};
      if (ppm < 1200) return {cls:'warn', label:'Moderate'};
      return {cls:'bad', label:'High'};
    }

    // PM2.5 and PM10 in µg/m³ (practical indoor heuristics)
    function bandPm25(x){
      if (!isFinite(x)) return {cls:'neutral', label:'—'};
      if (x < 12) return {cls:'good', label:'Good'};
      if (x < 35) return {cls:'warn', label:'Elevated'};
      return {cls:'bad', label:'High'};
    }
    function bandPm10(x){
      if (!isFinite(x)) return {cls:'neutral', label:'—'};
      if (x < 20) return {cls:'good', label:'Good'};
      if (x < 50) return {cls:'warn', label:'Elevated'};
      return {cls:'bad', label:'High'};
    }

    // Sensirion VOC/NOx indices: typical "good" around 100 baseline; higher indicates worse air
    function bandVoc(idx){
      if (!isFinite(idx)) return {cls:'neutral', label:'—'};
      if (idx < 100) return {cls:'good', label:'Low'};
      if (idx < 200) return {cls:'warn', label:'Medium'};
      return {cls:'bad', label:'High'};
    }
    function bandNox(idx){
      if (!isFinite(idx)) return {cls:'neutral', label:'—'};
      if (idx < 20) return {cls:'good', label:'Low'};
      if (idx < 100) return {cls:'warn', label:'Medium'};
      return {cls:'bad', label:'High'};
    }

    function bandHumidity(rh){
      if (!isFinite(rh)) return {cls:'neutral', label:'—'};
      if (rh < 30) return {cls:'warn', label:'Dry'};
      if (rh <= 60) return {cls:'good', label:'Good'};
      return {cls:'warn', label:'Humid'};
    }

    function bandTempC(tC){
      if (!isFinite(tC)) return {cls:'neutral', label:'—'};
      if (tC < 18) return {cls:'warn', label:'Cool'};
      if (tC <= 26) return {cls:'good', label:'Comfort'};
      return {cls:'warn', label:'Warm'};
    }

    function bandLux(lux){
      if (!isFinite(lux)) return {cls:'neutral', label:'—'};
      if (lux < 50) return {cls:'neutral', label:'Dim'};
      if (lux < 300) return {cls:'neutral', label:'Indoor'};
      return {cls:'neutral', label:'Bright'};
    }

    function setTag(el, band){
      el.className = 'tag ' + (band.cls || 'neutral');
      el.textContent = band.label || '—';
    }

    function setCardBand(card, band){
      card.classList.remove('good','warn','bad');
      if (band.cls === 'good') card.classList.add('good');
      else if (band.cls === 'warn') card.classList.add('warn');
      else if (band.cls === 'bad') card.classList.add('bad');
    }

    function overallAQ(b){
      // b includes bands for multiple metrics; worst-case drives overall.
      // If sensors unhealthy, we warn.
      if (!b.shtOk || !b.s88Ok || !b.senOk) {
        return {cls:'warn', title:'Moderate', advice:'One or more sensors report unhealthy status.'};
      }

      const order = {good:0, neutral:1, warn:2, bad:3};
      const worst = Math.max(order[b.co2.cls] ?? 1, order[b.pm25.cls] ?? 1, order[b.voc.cls] ?? 1);

      if (worst >= 3) return {cls:'bad', title:'Poor', advice:'One or more air-quality indicators are high.'};
      if (worst >= 2) return {cls:'warn', title:'Moderate', advice:'Some indicators are elevated. Monitor ventilation/activities.'};
      return {cls:'good', title:'Good', advice:'Conditions look good.'};
    }

    function setBanner(b){
      $('aqStatus').textContent = b.title;
      $('aqAdvice').textContent = b.advice;

      const pill = $('aqPill');
      pill.className = 'pill ' + b.cls;
      pill.innerHTML = '<span class="dot ' + (b.cls==='good'?'ok':(b.cls==='bad'?'bad':'')) + '"></span>' + b.title;
    }

    // -----------------------------
    // Formatting
    // -----------------------------
    function fmt2(x){ return (isFinite(x) ? x.toFixed(2) : '—'); }
    function fmt1(x){ return (isFinite(x) ? x.toFixed(1) : '—'); }

    // -----------------------------
    // Charts (canvas), lightweight
    // -----------------------------
    const HISTORY_MAX = 600; // 600 points @2s ~20 minutes
    const hist = {
      pm25: [],
      voc: []
    };

    function pushHist(arr, v){
      if (!isFinite(v)) return;
      arr.push(v);
      if (arr.length > HISTORY_MAX) arr.shift();
    }

    function minmax(arr){
      if (!arr.length) return {min:0,max:1};
      let mn = arr[0], mx = arr[0];
      for (let i=1;i<arr.length;i++){
        const v = arr[i];
        if (v < mn) mn = v;
        if (v > mx) mx = v;
      }
      if (mn === mx){ mn -= 1; mx += 1; }
      return {min:mn, max:mx};
    }

    function drawChart(canvas, arr, opts){
      const ctx = canvas.getContext('2d');
      const w = canvas.width, h = canvas.height;

      ctx.clearRect(0,0,w,h);

      const padL = 34, padR = 10, padT = 10, padB = 22;
      const gw = w - padL - padR;
      const gh = h - padT - padB;

      // Grid
      ctx.strokeStyle = 'rgba(148,163,184,0.16)';
      ctx.lineWidth = 1;
      ctx.beginPath();
      for (let i=0;i<=4;i++){
        const y = padT + (gh * i/4);
        ctx.moveTo(padL, y);
        ctx.lineTo(padL+gw, y);
      }
      ctx.stroke();

      if (arr.length < 3){
        ctx.fillStyle = 'rgba(148,163,184,0.70)';
        ctx.font = '14px ui-sans-serif, system-ui';
        ctx.fillText('Collecting history…', padL+10, padT+24);
        return;
      }

      const mm = minmax(arr);
      let mn = mm.min, mx = mm.max;

      if (opts && isFinite(opts.min)) mn = opts.min;
      if (opts && isFinite(opts.max)) mx = opts.max;
      if (mn === mx){ mn -= 1; mx += 1; }

      // Axis labels (min/max)
      ctx.fillStyle = 'rgba(148,163,184,0.85)';
      ctx.font = '12px ui-sans-serif, system-ui';
      const dec = opts?.decimals ?? 0;
      ctx.fillText(mn.toFixed(dec), 4, padT + gh);
      ctx.fillText(mx.toFixed(dec), 4, padT + 12);

      // Threshold bands (optional)
      if (opts?.bands?.length){
        for (const b of opts.bands){
          const y1 = padT + gh - gh * ((b.min - mn) / (mx - mn));
          const y2 = padT + gh - gh * ((b.max - mn) / (mx - mn));
          const top = Math.min(y1,y2), bot = Math.max(y1,y2);
          ctx.fillStyle = b.fill;
          ctx.fillRect(padL, top, gw, bot-top);
        }
      }

      // Line
      ctx.strokeStyle = 'rgba(59,130,246,0.95)';
      ctx.lineWidth = 2;
      ctx.beginPath();
      for (let i=0;i<arr.length;i++){
        const x = padL + (gw * i/(arr.length-1));
        const v = arr[i];
        const yNorm = (v - mn) / (mx - mn);
        const y = padT + gh - (gh * yNorm);
        if (i === 0) ctx.moveTo(x,y);
        else ctx.lineTo(x,y);
      }
      ctx.stroke();

      // Latest marker
      const last = arr[arr.length-1];
      const x = padL + gw;
      const yNorm = (last - mn) / (mx - mn);
      const y = padT + gh - (gh * yNorm);
      ctx.fillStyle = 'rgba(59,130,246,1.0)';
      ctx.beginPath();
      ctx.arc(x, y, 3.5, 0, Math.PI*2);
      ctx.fill();
    }

    function redrawCharts(){
      // PM2.5: draw with bands corresponding to good/elevated/high
      drawChart($('chPm25'), hist.pm25, {
        decimals: 1,
        bands: [
          {min: -1e9, max: 12, fill: 'rgba(34,197,94,0.08)'},
          {min: 12, max: 35, fill: 'rgba(245,158,11,0.08)'},
          {min: 35, max: 1e9, fill: 'rgba(239,68,68,0.06)'},
        ]
      });

      // VOC index: baseline ~100; bands <100 low, 100–200 medium, >200 high
      drawChart($('chVoc'), hist.voc, {
        decimals: 0,
        bands: [
          {min: -1e9, max: 100, fill: 'rgba(34,197,94,0.08)'},
          {min: 100, max: 200, fill: 'rgba(245,158,11,0.08)'},
          {min: 200, max: 1e9, fill: 'rgba(239,68,68,0.06)'},
        ]
      });

      // Update ranges
      if (hist.pm25.length){
        const mm = minmax(hist.pm25);
        $('pm25Range').textContent = 'Range: ' + mm.min.toFixed(1) + '–' + mm.max.toFixed(1) + ' µg/m³';
      } else $('pm25Range').textContent = '—';

      if (hist.voc.length){
        const mm = minmax(hist.voc);
        $('vocRange').textContent = 'Range: ' + Math.round(mm.min) + '–' + Math.round(mm.max);
      } else $('vocRange').textContent = '—';
    }

    // -----------------------------
    // Main polling loop
    // -----------------------------
    async function tick(){
      try{
        const r = await fetch('/json', {cache:'no-store'});
        const j = await r.json();

        lastOkMs = Date.now();
        setConnection(true);
        $('lastUpdate').textContent = new Date().toLocaleTimeString();

        // Parse fields (exact keys from your JSON)
        const ms   = Number(j.uptime_ms);
        const tC   = Number(j.temperature_C);
        const rh   = Number(j.humidity_pct);
        const lux  = Number(j.lux);
        const als  = Number(j.als);
        const co2  = Number(j.co2_ppm);

        const pm25 = Number(j.pm2_5_ugm3);
        const pm10 = Number(j.pm10_ugm3);
        const voc  = Number(j.voc_index);
        const nox  = Number(j.nox_index);

        const shtOk  = !!j.temp_rh_ok;
        const vemlOk = !!j.veml7700_ok;
        const s88Ok  = !!j.s88_ok;
        const senOk  = !!j.sen55_ok;

        // Display values
        const tDisp = useF ? cToF(tC) : tC;
        $('t').textContent    = fmt2(tDisp);
        $('rh').textContent   = fmt2(rh);
        $('lux').textContent  = fmt2(lux);
        $('als').textContent  = isFinite(als) ? als : '—';
        $('ms').textContent   = isFinite(ms) ? ms : '—';
        $('co2').textContent  = isFinite(co2) ? Math.round(co2) : '—';

        $('pm25').textContent = fmt1(pm25);
        $('pm10').textContent = fmt1(pm10);
        $('voc').textContent  = isFinite(voc) ? Math.round(voc) : '—';
        $('nox').textContent  = isFinite(nox) ? Math.round(nox) : '—';

        // Health tags
        $('shtok').textContent = 'SEN55 T/RH: ' + (shtOk ? 'OK' : 'ERR');
        $('shtok').className   = 'tag ' + (shtOk ? 'good' : 'bad');

        $('vemlok').textContent = 'VEML7700: ' + (vemlOk ? 'OK' : 'ERR');
        $('vemlok').className   = 'tag ' + (vemlOk ? 'good' : 'bad');

        $('s88ok').textContent  = 'S88: ' + (s88Ok ? 'OK' : 'ERR');
        $('s88ok').className    = 'tag ' + (s88Ok ? 'good' : 'bad');

        $('sen55ok').textContent = 'SEN55: ' + (senOk ? 'OK' : 'ERR');
        $('sen55ok').className   = 'tag ' + (senOk ? 'good' : 'bad');

        // Bands
        const bCo2  = bandCo2(co2);
        const bPm25 = bandPm25(pm25);
        const bPm10 = bandPm10(pm10);
        const bVoc  = bandVoc(voc);
        const bNox  = bandNox(nox);

        const bRh   = bandHumidity(rh);
        const bT    = bandTempC(tC);
        const bLux  = bandLux(lux);

        setTag($('co2Tag'), bCo2);
        setTag($('pm25Tag'), bPm25);
        setTag($('pm10Tag'), bPm10);
        setTag($('vocTag'), bVoc);
        setTag($('noxTag'), bNox);

        setTag($('rhTag'), bRh);
        setTag($('tTag'), bT);
        setTag($('luxTag'), bLux);

        setCardBand($('cardCo2'), bCo2);
        setCardBand($('cardPm25'), bPm25);
        setCardBand($('cardPm10'), bPm10);
        setCardBand($('cardVoc'), bVoc);
        setCardBand($('cardNox'), bNox);
        setCardBand($('cardRh'), bRh);
        setCardBand($('cardTemp'), bT);

        // Overall banner (uses CO2 + PM2.5 + VOC + sensor health)
        const overall = overallAQ({
          co2: bCo2,
          pm25: bPm25,
          voc: bVoc,
          shtOk, s88Ok, senOk
        });
        setBanner(overall);

        // History + charts (PM2.5 & VOC)
        if (senOk) {
          pushHist(hist.pm25, pm25);
          pushHist(hist.voc, voc);
        }
        redrawCharts();

        setStale(false);
      } catch(e){
        setConnection(false);
        const stale = (Date.now() - lastOkMs) > STALE_AFTER_MS;
        setStale(stale);
      }
    }

    // Start
    tick();
    setInterval(tick, 2000);

    // Stale watchdog
    setInterval(() => {
      const stale = (Date.now() - lastOkMs) > STALE_AFTER_MS;
      setStale(stale);
    }, 1000);

    // Redraw charts on resize
    window.addEventListener('resize', redrawCharts);

  </script>
  </body>
  </html>)HTML";

  server.send(200, "text/html; charset=utf-8", page);
}



static void handleJson() {
  JsonDocument doc;

  doc["uptime_ms"]       = g.ms;
  doc["temperature_C"]   = isnan(g.tC) ? 0 : g.tC;
  doc["humidity_pct"]    = isnan(g.rh) ? 0 : g.rh;
  doc["lux"]             = isnan(g.lux) ? 0 : g.lux;
  doc["white"]           = isnan(g.white) ? 0 : g.white;
  doc["als"]             = g.als;
  doc["co2_ppm"]         = g.co2_ppm;
  doc["temp_rh_ok"]      = g.sen55_ok;
  doc["sht45_ok"]        = g.sen55_ok; // legacy dashboard field
  doc["veml7700_ok"]     = g.veml_ok;
  doc["s88_ok"]          = g.s88_ok;
  doc["sen55_ok"]        = g.sen55_ok;

  doc["pm1_0_ugm3"]     = isnan(g.pm1_0) ? 0 : g.pm1_0;
  doc["pm2_5_ugm3"]     = isnan(g.pm2_5) ? 0 : g.pm2_5;
  doc["pm4_0_ugm3"]     = isnan(g.pm4_0) ? 0 : g.pm4_0;
  doc["pm10_ugm3"]      = isnan(g.pm10_0) ? 0 : g.pm10_0;
  doc["sen55_temperature_C"] = isnan(g.sen55_tC) ? 0 : g.sen55_tC;
  doc["sen55_humidity_pct"]  = isnan(g.sen55_rh) ? 0 : g.sen55_rh;
  doc["voc_index"]      = isnan(g.voc_index) ? 0 : g.voc_index;
  doc["nox_index"]      = isnan(g.nox_index) ? 0 : g.nox_index;


  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

static void handleMetrics() {
  String out;
  out.reserve(256);

  out += "uptime_ms " + String(g.ms) + "\n";
  out += "temperature_C " + String(g.tC, 2) + "\n";
  out += "humidity_pct " + String(g.rh, 2) + "\n";
  out += "lux " + String(g.lux, 2) + "\n";
  out += "white " + String(g.white, 2) + "\n";
  out += "als " + String(g.als) + "\n";
  out += "co2_ppm " + String(g.co2_ppm) + "\n";
  out += "temp_rh_ok " + String(g.sen55_ok ? 1 : 0) + "\n";
  out += "veml7700_ok " + String(g.veml_ok ? 1 : 0) + "\n";
  out += "s88_ok " + String(g.s88_ok ? 1 : 0) + "\n";
  out += "wifi_ok " + String((WiFi.status() == WL_CONNECTED) ? 1 : 0) + "\n";
  out += "sen55_ok " + String(g.sen55_ok ? 1 : 0) + "\n";
  out += "pm1_0_ugm3 " + String(g.pm1_0, 2) + "\n";
  out += "pm2_5_ugm3 " + String(g.pm2_5, 2) + "\n";
  out += "pm4_0_ugm3 " + String(g.pm4_0, 2) + "\n";
  out += "pm10_ugm3 "  + String(g.pm10_0, 2) + "\n";
  out += "voc_index "  + String(g.voc_index, 2) + "\n";
  out += "nox_index "  + String(g.nox_index, 2) + "\n";

  server.send(200, "text/plain; charset=utf-8", out);
}

namespace WebServerMgr {

void begin() {
  server.on("/", handleRoot);
  server.on("/json", handleJson);
  server.on("/metrics", handleMetrics);

  server.onNotFound([]() {
    server.send(404, "text/plain", "Not found");
  });

  server.begin();
  Serial.println("HTTP server started on port 80");
}

void tick() {
  server.handleClient();
}

} // namespace WebServerMgr
