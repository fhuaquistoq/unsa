# StreamForest: Persistent Event Memory

**Paper:** StreamForest: Efficient Online Video Understanding with Persistent Event Memory  
**Objetivo:** comprender video online con memoria persistente de eventos y percepcion fina del presente.  
**Tipo de solucion:** arquitectura entrenada/fine-tuned, memoria jerarquica de eventos.

## Respuesta corta

StreamForest separa lo que debe verse con detalle ahora de lo que debe recordarse del pasado. Importa porque propone una arquitectura de streaming con **FSTW** para percepcion fina actual y memoria corta, y **PEMF** para memoria persistente de eventos. Usalo como el pipeline conceptualmente mas fuerte del survey; para ejecucion practica, empieza con checkpoints y evita entrenar desde cero.

## Problema que ataca

En streaming, el modelo necesita tres capacidades al mismo tiempo:

1. Percibir el presente con alta resolucion.
2. Mantener memoria corta de cambios recientes.
3. Recordar eventos pasados sin guardar todos los frames/tokens.

StreamForest representa el pasado como eventos persistentes organizados en arboles, no como una lista plana de frames.

## Arquitectura

| Bloque | Rol | Que guarda |
|---|---|---|
| FSTW | Percepcion de alta resolucion y memoria corta | Frame actual + memoria espaciotemporal reciente |
| PEMF | Memoria persistente de largo plazo | Meta-eventos organizados en un bosque de arboles |
| OnlineIT | Fine-tuning para streaming | Datos de instruccion general y driving |
| ODV-Bench | Evaluacion de driving online | Preguntas de percepcion, prediccion y riesgo |

## FSTW vs. PEMF

**FSTW** tiene dos partes: percepcion real-time del frame actual con alta resolucion y memoria espaciotemporal corta. Cuando la memoria corta excede su capacidad, el overflow se segmenta en meta-eventos y pasa a **PEMF**.

**PEMF** comprime el pasado a nivel de evento. Fusiona nodos usando penalizaciones por similitud, distancia temporal y cantidad de merges, para conservar contenido informativo sin crecimiento lineal.

## Pipeline general

1. **Stream a 1 FPS:** la evaluacion procesa frames en streaming a 1 FPS.
2. **FSTW:** captura percepcion actual y memoria corta espaciotemporal.
3. **Segmentacion de meta-eventos:** usa minimos locales de similitud inter-frame.
4. **PEMF:** inserta eventos en un bosque persistente.
5. **Merge adaptativo:** fusiona eventos por similitud, distancia temporal y frecuencia de merges.
6. **Control de tokens:** cap visual por defecto de 8192 tokens.
7. **Respuesta:** el MLLM usa raices de PEMF + visual features de FSTW.

## OnlineIT y entrenamiento

| Elemento | Dato |
|---|---|
| OnlineIT-general | Dataset de instruction tuning para streaming general |
| OnlineIT-drive | 89K instancias de streaming QA para conduccion autonoma |
| Training completo | 5 etapas |
| Recursos de training | 32 A100 GPUs |
| Etapa opcional | Fine-tuning con OnlineIT-drive produce StreamForest (FT-drive) |

## Resultados headline

| Resultado | Valor |
|---|---:|
| StreamingBench | 77.3% |
| OVBench | 60.5% |
| OVO-Bench | 55.6% |
| Visual token cap por defecto | 8192 |
| Cap comprimido extremo | 1024 |
| Performance retenida con 1024 tokens | 96.8% promedio |

## Repositorio y datasets

- **Repositorio oficial:** https://github.com/MCG-NJU/StreamForest
- **Homepage:** https://happyzqy.github.io/StreamForest/
- **Paper:** https://arxiv.org/abs/2509.24871 *(URL no verificada desde el Docling local)*
- **Modelos y datos:** https://huggingface.co/collections/MCG-NJU/streamforest-and-odvbench-68da5d8a88f3cf453bca0e09
- **Datasets/benchmarks:** OnlineIT, ODV-Bench, StreamingBench, OVBench, OVO-Bench, VideoMME, MLVU, MVBench, PerceptionTest.

## Nota de ejecucion

**Factibilidad: media para evaluacion con checkpoints; baja para entrenamiento completo.** El repo tiene instalacion, checkpoints y scripts, pero entrenar todo implica cinco etapas y 32 A100. Para investigacion practica conviene evaluar checkpoints y no intentar reproducir training desde cero al inicio.

Comandos base indicados:

```bash
conda create --name StreamForest python=3.10
conda activate StreamForest
pip install -r requirements.txt
python3 download_hf.py
bash scripts/eval/run_eval.sh
```

Hay que reemplazar rutas `/your_local_path_to` por rutas locales reales.

## Fortalezas

- Arquitectura conceptualmente fuerte: presente fino + pasado comprimido por eventos.
- PEMF es mas semantico que un buffer plano de frames/KV.
- Reporta robustez bajo compresion extrema de tokens.
- Incluye datos y benchmark orientados a streaming y conduccion.

## Debilidades

- Entrenamiento completo es muy costoso.
- Requiere mapear datasets/videos locales para varias tareas.
- Es mas complejo de explicar y depurar que ReKV o rLiVS.
- La calidad depende de segmentacion/fusion de eventos y del fine-tuning.

## Riesgo de reproduccion

- [ ] Usar checkpoints antes de intentar entrenamiento.
- [ ] Confirmar descarga de modelos, OnlineIT/ODV-Bench y anotaciones.
- [ ] Reemplazar todas las rutas `/your_local_path_to`.
- [ ] Mantener 1 FPS si se quiere comparar contra el paper.
- [ ] Registrar token cap usado: 8192 por defecto o 1024 comprimido.
- [ ] No presentar training completo como factible sin 32 A100.

## Veredicto para el survey

StreamForest es el pipeline mas interesante conceptualmente. Para ejecutar rapido, ReKV es mejor. Para construir una narrativa fuerte sobre **memoria persistente de eventos en Video LLM streaming**, StreamForest es el candidato mas rico.
