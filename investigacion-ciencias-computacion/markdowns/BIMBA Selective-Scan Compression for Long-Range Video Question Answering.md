BIMBA: Selective-Scan Compression for Long-Range Video Question Answering
|     |     | MdMohaiminulIslam1* |     |                 |     | TusharNagarajan2 |                   |     | HuiyuWang2 |     |     |     |     |     |
| --- | --- | ------------------- | --- | --------------- | --- | ---------------- | ----------------- | --- | ---------- | --- | --- | --- | --- | --- |
|     |     |                     |     | GedasBertasius1 |     |                  | LorenzoTorresani2 |     |            |     |     |     |     |     |
|     |     |                     |     | 1UNCChapelHill  |     |                  | 2MetaAI           |     |            |     |     |     |     |     |
https://sites.google.com/view/bimba-mllm
5202 raM 31  ]VC.sc[  2v09590.3052:viXra
Abstract video inputs spanning a few seconds. In this case, a sim-
plebuteffectivestrategyinvolvesextracting“tokens”from
Video Question Answering (VQA) in long videos poses the individual frames of the video using a pretrained im-
the key challenge of extracting relevant information and age encoder [3, 34, 35, 45, 52, 65]. The concatenation
modeling long-range dependencies from many redundant of the tokens extracted from the frames is then passed to
frames. The self-attention mechanism provides a general theLLMforsemanticprocessingandlanguagegeneration.
Thisstrategybreaksdownwhenthevideoisverylong,e.g.,
solutionforsequencemodeling,butithasaprohibitivecost
when applied to a massive number of spatiotemporal to- when it spans minutes or hours, because of the quadratic
kens in long videos. Most prior methods rely on compres- cost of the self-attention operation needed to process the
|     |     |     |     |     |     |     | extractedvisualtokens. |     |     | Inaddition,naiveconcatenationof |     |     |     |     |
| --- | --- | --- | --- | --- | --- | --- | ---------------------- | --- | --- | ------------------------------- | --- | --- | --- | --- |
sionstrategiestolowerthecomputationalcost,suchasre-
ducingtheinputlengthviasparseframesamplingorcom- frame-level tokens for a long video would flood the LLM
pressing the output sequence passed to the large language with an exceedingly long and redundant sequence of fea-
model(LLM)viaspace-timepooling. However,thesenaive tures. For example, the LLAMA-3.2 [52] image encoder
approaches over-represent redundant information and of- outputs1600∼6400tokensperimage. Therefore, encod-
|          |         |           |                |            |     |      | ing just | 128 frames | would |     | produce | a sequence |     | of 205K ∼ |
| -------- | ------- | --------- | -------------- | ---------- | --- | ---- | -------- | ---------- | ----- | --- | ------- | ---------- | --- | --------- |
| ten miss | salient | events or | fast-occurring | space-time |     | pat- |          |            |       |     |         |            |     |           |
terns. Inthiswork,weintroduceBIMBA,anefficientstate- 820Ktokens,whichisimpracticaltoprocessevenbymod-
| spacemodeltohandlelong-formvideos. |     |     |     | Ourmodellever- |     |     | ernGPUs. |     |     |     |     |     |     |     |
| ---------------------------------- | --- | --- | --- | -------------- | --- | --- | -------- | --- | --- | --- | --- | --- | --- | --- |
agestheselectivescanalgorithmtolearntoeffectivelyse- To address these issues, most long-form video MLLMs
| lect critical | information |     | from high-dimensional |     | video | and |                   |     |            |     |           |     |            |        |
| ------------- | ----------- | --- | --------------------- | --- | ----- | --- | ----------------- | --- | ---------- | --- | --------- | --- | ---------- | ------ |
|               |             |     |                       |     |       |     | adopt compression |     | techniques |     | to reduce |     | the number | of vi- |
transform it into a reduced token sequence for efficient sual tokens. For example, the methods in [50, 75] ap-
| LLM processing. |     | Extensive | experiments | demonstrate |     | that |            |                  |     |     |         |                |     |        |
| --------------- | --- | --------- | ----------- | ----------- | --- | ---- | ---------- | ---------------- | --- | --- | ------- | -------------- | --- | ------ |
|                 |     |           |             |             |     |      | ply simple | spatial/temporal |     |     | pooling | to frame-level |     | tokens |
BIMBAachievesstate-of-the-artaccuracyonmultiplelong-
|     |     |     |     |     |     |     | to shorten | the | sequence | passed | to  | the LLM. | However, | the |
| --- | --- | --- | --- | --- | --- | --- | ---------- | --- | -------- | ------ | --- | -------- | -------- | --- |
form VQA benchmarks, including PerceptionTest, NExT- pooling operation discards important spatiotemporal infor-
QA,EgoSchema,VNBench,LongVideoBench,Video-MME,
|     |     |     |     |     |     |     | mation. | Analternativestrategyistouseconvolution-based |     |     |     |     |     |     |
| --- | --- | --- | --- | --- | --- | --- | ------- | --------------------------------------------- | --- | --- | --- | --- | --- | --- |
https:
and MLVU. Code and models are available at modules to simultaneously perform sequence compression
//sites.google.com/view/bimba-mllm. and temporal modeling [42, 47]. However, convolution-
|     |     |     |     |     |     |     | based models |         | lack long-range |      | modeling |       | ability     | since con- |
| --- | --- | --- | --- | --- | --- | --- | ------------ | ------- | --------------- | ---- | -------- | ----- | ----------- | ---------- |
|     |     |     |     |     |     |     | volutional   | kernels | can             | only | capture  | local | short-range | de-        |
1.Introduction
|                |                     |             |            |               |         |          | pendencies. | Recent   |         | work [80] | reduces        | the   | number    | of out-   |
| -------------- | ------------------- | ----------- | ---------- | ------------- | ------- | -------- | ----------- | -------- | ------- | --------- | -------------- | ----- | --------- | --------- |
|                |                     |             |            |               |         |          | put visual  | tokens   | by      | applying  | self-attention |       | with      | a smaller |
| Large Language |                     | Models      | (LLMs) [1, | 5, 11,        | 14, 30, | 53,      |             |          |         |           |                |       |           |           |
|                |                     |             |            |               |         |          | number      | of query | keys    | [29].     | However,       | these | systems   | trade     |
| 54, 64]        | have revolutionized |             | the field  | of artificial |         | intelli- |             |          |         |           |                |       |           |           |
|                |                     |             |            |               |         |          | efficiency  | at the   | expense | of        | cross-frame    |       | analysis, | which is  |
| gence and      | produced            | significant | changes    | in            | various | fields.  |             |          |         |           |                |       |           |           |
essentialforlongvideounderstanding.
| Building | on LLMs, | Multimodal | Large               | Language | Models |         |         |       |     |         |               |     |     |             |
| -------- | -------- | ---------- | ------------------- | -------- | ------ | ------- | ------- | ----- | --- | ------- | ------------- | --- | --- | ----------- |
|          |          |            |                     |          |        |         | In this | work, | we  | propose | Bidirectional |     |     | Interleaved |
| (MLLMs)  | have     | recently   | enabled significant |          | gains  | for im- |         |       |     |         |               |     |     |             |
age[2,4,9,12,24,34–36,43,45,52,77,88]andvideoun- Mamba for Better Answers (BIMBA), which provides an
|             |           |           |              |     |             |     | efficient | and    | effective | alternative      |     | for compressing |     | long      |
| ----------- | --------- | --------- | ------------ | --- | ----------- | --- | --------- | ------ | --------- | ---------------- | --- | --------------- | --- | --------- |
| derstanding | problems, | typically | reformulated |     | as language |     |           |        |           |                  |     |                 |     |           |
|             |           |           |              |     |             |     | videos    | into a | short,    | information-rich |     | sequence        |     | of tokens |
generationfromvideo[7,38,39,42,46,50,60,68,80].
|     |     |     |     |     |     |     | for video | question-answering |     |     | by LLMs. |     | Our system | is in- |
| --- | --- | --- | --- | --- | --- | --- | --------- | ------------------ | --- | --- | -------- | --- | ---------- | ------ |
However,mostvideoMLLMshavebeenappliedtoshort
|     |     |     |     |     |     |     | spired by | the | success | of state-space |     | models | (SSMs) | [18– |
| --- | --- | --- | --- | --- | --- | --- | --------- | --- | ------- | -------------- | --- | ------ | ------ | ---- |
*WorkdoneduringaninternshipatMeta. 22]forprocessinglongdocumentsinNLP[17]. SSMsin-
1

herentlypossesslong-termmodelingabilitywhilerequiring forLLMs. Therefore, manyexistingmodelscanonlypro-
linearcomputationcostswithsequencelengthinsteadofthe cess a limited number of video frames [3, 10, 34, 38, 82].
quadratic cost of self-attention. In particular, Mamba [17] Other methods utilize compression techniques to reduce
furtherimprovedSSMswithaselectionmechanismthatal- the number of tokens before passing them to the LLMs.
lows the model to select relevant information in an input- For example, Video-ChatGPT employs pooling modules
dependent manner. We design a video architecture based to reduce data dimensions, MovieChat [63] and Chat-
around the Mamba module to compress the sequence of UniVi [31] adopt memory-based mechanisms, LLaMA-
spatiotemporaltokensbymorethanoneorderofmagnitude VID [41] learns condensed representations using dual to-
(e.g.,from102Ktokensto6.4ktokens)whileretaininges- kens. Most existing token compression techniques are
sentiallong-rangedependencies.Theselective-scanmecha- basedonpooling,perceiver,orQformeroperations. These
nism[17]ofourtokenselectormoduleallowsthemodelto operations lack the ability to perform long-range model-
selectively propagate important tokens and discard redun- ing and capture effective information from long input se-
dantonesfromthehighlyredundantvideocontenttypically quences. In contrast, we introduce a Mamba-based to-
presentinlongvideos. kencompressionmethodthatefficientlymodelslong-range
|          |         |       |        |           |          | spatiotemporal |     | dependencies, | selectively |     | capturing | essen- |
| -------- | ------- | ----- | ------ | --------- | -------- | -------------- | --- | ------------- | ----------- | --- | --------- | ------ |
| Although | SSM and | Mamba | models | have been | used be- |                |     |               |             |     |           |        |
fore for image analysis [8, 56, 56, 89], image-language tial information while discarding redundancies to create a
modeling [25, 33, 58, 84] and video understanding [6, 26, moreeffectivecompressedvideorepresentation.
27,40,49,76],ourproposedarchitecturediffersfromthese
2.2.StateSpaceModels
priorworksbyintroducingafewsimplebuteffectivecon-
tributions. First, we introduce an SSM-based compression Inspired by classical state space theory [32], state space
module called a spatiotemporal token selector, which can models(SSMs)havebeenproposedforbuildingdeeplearn-
takealargenumberofspatiotemporalvideotokensasinput ingmodelsthatefficientlyhandlelong-sequencedata[18–
and output a significantly smaller number of compressed 20]. Afterward, several methods have been proposed for
tokens only containing important information by utilizing long-sequencemodelingusingSSMs,includingStructured
theselective-scan mechanism[17]. We achievethisby in- State Space (S4) Models [20] and Diagonal State Space
troducing a smaller number of visual queries and jointly (DSS)[22]. Recently, Mamba [17] has advanced S4 by
modelingthemwiththespatiotemporalvideotokens. Sec- adding a time-varying, input-dependent selection mecha-
ond, we propose an improved method for concatenating nism, providing efficient scaling for NLP and achieving
visual and spatiotemporal tokens by interleaving them at transformer-levelperformanceinlong-sequencetasks. Fur-
equal intervals. The standard approach of appending to- thermore, SSMs have been successfully applied to several
kens at the end can introduce positional bias, where end- image[8,56,56,89]andvideo[6,26,27,40,49,76]anal-
sequencetokensdisproportionatelyinfluencequerytokens. ysistasks,aswellastoimageMLLMs[58,84].Inspiredby
thiswork,weproposetoleverageSSMs/Mambatodevelop
| Ourinterleavedpositioningmitigatesthisbias, |     |     |     |     | influencing |     |     |     |     |     |     |     |
| ------------------------------------------- | --- | --- | --- | --- | ----------- | --- | --- | --- | --- | --- | --- | --- |
queries more uniformly across the entire video sequence. effectivelong-formvideoMLLMs.
| Finally,     | we use a bidirectional |              | selective-scan |                | strategy [40] |               |     |            |     |     |     |     |
| ------------ | ---------------------- | ------------ | -------------- | -------------- | ------------- | ------------- | --- | ---------- | --- | --- | --- | --- |
|              |                        |              |                |                |               | 3.Background: |     | MambaModel |     |     |     |     |
| that is more | effective              | in capturing | 2D/3D          | spatiotemporal |               |               |     |            |     |     |     |     |
structuresinvideocomparedtothestandardselectivescan,
|     |     |     |     |     |     | The Mamba | model | (also | known | as Selective | Scan | Struc- |
| --- | --- | --- | --- | --- | --- | --------- | ----- | ----- | ----- | ------------ | ---- | ------ |
whichisbettersuitedfor1Dsequencemodeling.
|               |     |              |       |          |         | tured State          | Space | or  | S6) [17]   | is a recently |          | introduced |
| ------------- | --- | ------------ | ----- | -------- | ------- | -------------------- | ----- | --- | ---------- | ------------- | -------- | ---------- |
| We experiment | on  | several long | video | question | answer- |                      |       |     |            |               |          |            |
|               |     |              |       |          |         | sequence-to-sequence |       |     | model that | offers        | distinct | advan-     |
ingdatasetsincludingPerceptionTest[55],NExT-QA[72],
tagesoverexistingapproacheslikeconvolutionalnetworks
| EgoSchema | [51], VNBench | [85], | LongVideoBench |                  | [71], |         |                 |          |             |         |                |         |
| --------- | ------------- | ----- | -------------- | ---------------- | ----- | ------- | --------------- | -------- | ----------- | ------- | -------------- | ------- |
|           |               |       |                |                  |       | (CNNs), | recurrent       | networks | (RNNs),     | and     | self-attention |         |
| Video-MME | [15], MLVU    | [86], | and show       | state-of-the-art |       |         |                 |          |             |         |                |         |
|           |               |       |                |                  |       | models  | (Transformers). |          | Traditional | models, | such           | as con- |
resultsonallofthesedatasets.
|     |     |     |     |     |     | volution | and recurrence, |            | are computationally |              | efficient | but        |
| --- | --- | --- | --- | --- | --- | -------- | --------------- | ---------- | ------------------- | ------------ | --------- | ---------- |
|     |     |     |     |     |     | struggle | to capture      | long-range |                     | dependencies |           | within se- |
2.RelatedWork
quences.Incontrast,self-attentionexcelsatmodelinglong-
|     |     |     |     |     |     | range dependencies |     | but | at a much | higher | computational |     |
| --- | --- | --- | --- | --- | --- | ------------------ | --- | --- | --------- | ------ | ------------- | --- |
2.1.VideoMulti-ModalLargeLanguageModels
|     |     |     |     |     |     | cost,whichscalesquadraticallywithsequencelength. |     |     |     |     |     | The |
| --- | --- | --- | --- | --- | --- | ------------------------------------------------ | --- | --- | --- | --- | --- | --- |
Recently, we have witnessed many advances in video S6 model combines the best of both worlds: it has the
MLLMs. Video LMMs process videos by encoding video capacity to model long-range dependencies in sequences
frames using image or video encoders and passing the ex- while maintaining computational efficiency, achieving lin-
tracted features to the LLMs. Unlike images, videos usu- earcomputationalcostandmemoryusage.
ally generate a large number of frame-based features (to- Theoretically,theState-SpaceModel(SSM)representsa
kens), which causes computation and context limitations continuous,time-invariantsystemthatmapsaninputsignal
2

Figure1. OurproposedBIMBAmodel usesaMamba-basedSpatiotemporalTokenSelectortoselectareducednumberofsalienttokens
fromalongsequenceoffeaturesextractedviaapretrainedimageencoder. Thetokenselectionisoptionallyconditionedusingthetextual
querytoidentifythefeaturesthataremostinformativeforansweringagivenquestion. Finally,theselectedandtransformedtokensare
passedtoalargelanguagemodelwithatokenizedversionoftheinputquestiontogeneratetheanswer.
x(t) ∈ RL to output y(t) ∈ RM through a hidden state 4.TechnicalApproach
RN.
| h(t) ∈ | Mathematically,SSMscanbedescribedusing |     |     |     |            |        |              |       |          |       |
| ------ | -------------------------------------- | --- | --- | --- | ---------- | ------ | ------------ | ----- | -------- | ----- |
|        |                                        |     |     |     | We present | BIMBA, | a multimodal | large | language | model |
asetoflinearordinarydifferentialequations(ODEs):
|     |     |     |     |     | (MLLM) designed | for | long-range | video | question | answer- |
| --- | --- | --- | --- | --- | --------------- | --- | ---------- | ----- | -------- | ------- |
h′(t)=Ah(t)+Bx(t),
|     |     |     |     |     | ing. Figure1illustratestheoverallarchitecture. |     |     |     |     | Ourmodel |
| --- | --- | --- | --- | --- | ---------------------------------------------- | --- | --- | --- | --- | -------- |
(1)
y(t)=Ch(t)+Dh(t). consists of a standard image encoder [13], a spatiotempo-
|             |     |                                      |     |     | raltokenselector,andanLLMdecoder[52]. |       |                     |     | First,ween- |         |
| ----------- | --- | ------------------------------------ | --- | --- | ------------------------------------- | ----- | ------------------- | --- | ----------- | ------- |
| Here,A∈RN×N |     | representsthestatematrixofthesystem, |     |     |                                       |       |                     |     |             |         |
|             |     |                                      |     |     | code each input                       | video | frame independently |     | using       | the im- |
| RN,C        | RN  |                                      |     | R1  |                                       |       |                     |     |             |         |
B ∈ ∈ areprojectionmatrices,andD ∈ is age encoder and extract patch-level features (tokens) from
theskipconnectionforastatesizeN.
|     |     |     |     |     | eachframe. | Then, weuseaspatiotemporaltokenselector, |     |     |     |     |
| --- | --- | --- | --- | --- | ---------- | ---------------------------------------- | --- | --- | --- | --- |
To apply SSMs to real-world data, the continuous whichreducesthesequenceofspatiotemporalinputtokens
ODE(1)isfirstdiscretizedusingthefollowingequation: bymorethananorderofmagnitude(e.g.,16×)forefficient
processingbythesubsequentLLM.Specifically,weutilize
|     |     | h t =Ah | k−1 +Bx k , |     |     |     |     |     |     |     |
| --- | --- | ------- | ----------- | --- | --- | --- | --- | --- | --- | --- |
(2) theselective-scanstructuredstate-spacemodel[17]forde-
|     |     | y t =Ch | k +Dx k . |     |                          |     |                              |     |     |     |
| --- | --- | ------- | --------- | --- | ------------------------ | --- | ---------------------------- | --- | --- | --- |
|     |     |         |           |     | signingourtokenselector. |     | Ourspatiotemporaltokenselec- |     |     |     |
Using this formulation, an efficient implementation of torefficientlymodelsthelong-rangespatiotemporaldepen-
denciesintheinputvideoandselectsonlythemostrelevant
| SSM, called | Structured | State | Space (S4), | has been pro- |     |     |     |     |     |     |
| ----------- | ---------- | ----- | ----------- | ------------- | --- | --- | --- | --- | --- | --- |
posed [20]. S4 represents the state matrix A as diagonal tokensfromthevastinputset. Finally, thecompressedto-
andlow-rank,whichfacilitatesefficientcomputation. kensequenceispassedtotheLLMalongwiththelanguage
The S4 model is a Linear Time-Invariant (LTI) system questiontogeneratetheresponse. Inthefollowingsubsec-
with parameters that remain constant and are independent tions, weprovide a detailed discussionof each component
ofourmodel.
| oftheinput. | Thiscanbesuboptimal,especiallyformodel- |     |     |     |     |     |     |     |     |     |
| ----------- | --------------------------------------- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
inglong-termdependenciesinasequencecontainingredun-
4.1.ImageEncoder
| dant and | unimportant | information. | Recent | work [17] pro- |     |     |     |     |     |     |
| -------- | ----------- | ------------ | ------ | -------------- | --- | --- | --- | --- | --- | --- |
poses Selective Scan State Space (S6) to develop Mamba We use a standard vision transformer [13] to encode each
LLM[17]. InS6,thematricesB,C,and∆aremadeinput videoframeindependently. LetV = (V ,...,V ,...,V ) ∈
|     |     |     |     |     |     |     |     |     | 1 t | T   |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
dependentandderivedfrominputxusinglinearlayers. RT×3×H×W be the input video consisting of T frames
R3×H×W
|     |     |          |     |     | V t ∈                     | wherethe3channelsencodecolorinRGB |     |                        |     |     |
| --- | --- | -------- | --- | --- | ------------------------- | --------------------------------- | --- | ---------------------- | --- | --- |
|     |     | B=Linear | (x) |     |                           |                                   |     |                        |     |     |
|     |     |          | N   |     | format,H istheheight,andW |                                   |     | isthewidthofeachframe. |     |     |
(3)
|                                                     |     | C=Linear | (x) |     |                                                     |     |                       |                    |     |     |
| --------------------------------------------------- | --- | -------- | --- | --- | --------------------------------------------------- | --- | --------------------- | ------------------ | --- | --- |
|                                                     |     |          | N   |     | First,wedivideeachframeintonon-overlappingpatchesof |     |                       |                    |     |     |
|                                                     |     |          |     |     | size(p×p).Then,animageencoderf                      |     |                       | isappliedtoextract |     |     |
| Suchaselective-scanapproachallowstheparameterstoin- |     |          |     |     |                                                     |     |                       | I                  |     |     |
|                                                     |     |          |     |     | spatialfeatures(tokens)z                            |     | ∈Rh×w×dfromeachframeV |                    |     | :   |
teract dynamically with the input across the sequence, en- t t
| abling the        | model | to retain                         | important information | and filter |     |     |       |     |     |     |
| ----------------- | ----- | --------------------------------- | --------------------- | ---------- | --- | --- | ----- | --- | --- | --- |
|                   |       |                                   |                       |            |     | z   | =f (V | ).  |     | (4) |
|                   |       |                                   |                       |            |     |     | t I   | t   |     |     |
| outredundantdata. |       | Asaresult,S6modelsarebettersuited |                       |            |     |     |       |     |     |     |
forapplicationsthatrequireefficientinformationcompres- where h = H/p, w = W/p, and d is the feature di-
sion,suchasours. mension. Afterward, the features of each frame are con-
3

Figure2. (a): ArchitectureofourSpatiotemporalTokenSelector. (b): Traditionalselectivescanwithqueriesappendedatthesequence’s
startorendintroducespositionalbiasesthatoftenleadtosuboptimalperformance. (c)Weproposetointerleavethequeriesuniformlyto
captureinteractionsbetweenspatiotemporaltokensacrossthevideomoreevenly. (d)Furthermore,weintroduceabidirectionalselective
scan(forwardandbackward)operationtoimprovethelong-rangemodelingfurther.
applylayernormalizationtoZ′followedbyaselective-scan
| catenated | to produce |     | the spatiotemporal |     | token | sequence |     |     |     |     |     |
| --------- | ---------- | --- | ------------------ | --- | ----- | -------- | --- | --- | --- | --- | --- |
Z ∈ RT×h×w×d. We denote the total number of tokens layer to capture long-range dependencies and critical fine-
byL=T ×h×w. grained information from input spatiotemporal tokens. A
|     |     |     |     |     |     |     | residual connection | is added | after | the selective-scan | layer. |
| --- | --- | --- | --- | --- | --- | --- | ------------------- | -------- | ----- | ------------------ | ------ |
4.2.SpatiotemporalTokenSelector Finally, the N output queries Q′ are extracted from the
combinedtokensequenceZ′andpassedtotheLLM.These
Thespatiotemporaltokenselectorhastwomainobjectives:
operationsareexpressedusingthefollowingequations:
(1)capturinglong-rangedependenciesfromthesequenceof
| space-time | tokens | produced | by  | the image | encoder | and (2) |     |     |     |     |     |
| ---------- | ------ | -------- | --- | --------- | ------- | ------- | --- | --- | --- | --- | --- |
Z′ =[Z;Q],
| selecting | the most | relevant | tokens | from | the highly | redun- |     |     |     |     |     |
| --------- | -------- | -------- | ------ | ---- | ---------- | ------ | --- | --- | --- | --- | --- |
Z =Z′
| dant information |     | typical | in long | videos. | This | task is chal- |     | res |     |     |     |
| ---------------- | --- | ------- | ------- | ------- | ---- | ------------- | --- | --- | --- | --- | --- |
lengingduetothelargenumberofspace-timetokensgen- Z′ =Selective-Scan(LN(Z′)) (5)
erated by the image encoder throughout the video. Using Z′ =Z +Z′
res
| conventional | self-attention |     | would | be  | computationally | pro- |     |     |     |     |     |
| ------------ | -------------- | --- | ----- | --- | --------------- | ---- | --- | --- | --- | --- | --- |
Q′ =Extract(Z′)
| hibitive | due to     | its quadratic |          | cost relative | to the          | sequence |              |           |          |        |              |
| -------- | ---------- | ------------- | -------- | ------------- | --------------- | -------- | ------------ | --------- | -------- | ------ | ------------ |
| length.  | To address | this          | problem, | we            | use a selective | scan-    |              |           |          |        |              |
|          |            |               |          |               |                 |          | Furthermore, | our token | selector | module | incorporates |
basedtokenselector[17],which(1)efficientlymodelslong-
|     |     |     |     |     |     |     | simple but | effective design | adaptations | to  | make it suit- |
| --- | --- | --- | --- | --- | --- | --- | ---------- | ---------------- | ----------- | --- | ------------- |
rangetemporaldependencieswithlinearcomputationalcost
|     |     |     |     |     |     |     | ableforprocessinglongvideoinputs, |     |     | asillustratedinFig- |     |
| --- | --- | --- | --- | --- | --- | --- | --------------------------------- | --- | --- | ------------------- | --- |
and(2)filtersoutredundanttokens,retainingonlythemost
ure2(c,d)anddescribednext.
relevantonesfromthelonginputsequence.
Figure 2(a) illustrates our spatiotemporal token selec- 4.2.1.InterleavedQueries
tor. First, we initialize a sequence of visual queries Q ∈ A straightforward way to concatenate queries with the
RT′×h′×w′×d
using an adaptive 3D average pooling layer space-time tokens is to append the queries at the end, as
| appliedtospatiotemporaltokensZ.Here,N |     |     |     |     | =T′×h′×w′ |     |                 |                |     |               |            |
| ------------------------------------- | --- | --- | --- | --- | --------- | --- | --------------- | -------------- | --- | ------------- | ---------- |
|                                       |     |     |     |     |           |     | shown in Figure | 2(b). However, |     | this approach | can intro- |
isthenumberofqueries,whichissignificantlysmallerthan ducebiasesasitpositionsthequeriesclosertotokensfrom
| thenumberofinputspatiotemporaltokensL=T |     |     |     |     |     | ×h×w, |                   |               |             |          |          |
| --------------------------------------- | --- | --- | --- | --- | --- | ----- | ----------------- | ------------- | ----------- | -------- | -------- |
|                                         |     |     |     |     |     |       | the later portion | of the video, | potentially | limiting | interac- |
| N <<                                    | L.  |     |     |     |     |       |                   |               |             |          |          |
i.e., In our implementation, the input consists tion with earlier frames and thus missing context from the
of 64×40×40 = 102,400 tokens, while the number of beginning of the video. To address this, we interleave the
| queriesis16×20×20=6,400. |     |     |     | Thus,theselectorapplies |     |     |               |                    |     |           |                |
| ------------------------ | --- | --- | --- | ----------------------- | --- | --- | ------------- | ------------------ | --- | --------- | -------------- |
|                          |     |     |     |                         |     |     | queries among | the spatiotemporal |     | tokens at | regular inter- |
a16×compressionratio. vals, as shown in Figure 2(c). By evenly distributing the
| Although | the | pooling | operation | provides | a good | initial- |     |     |     |     |     |
| -------- | --- | ------- | --------- | -------- | ------ | -------- | --- | --- | --- | --- | --- |
queriesacrossthesequence,ourdesignenablesinteraction
ization for visual queries, it does not capture long-range with tokens from all parts of the video, supporting a more
spatiotemporal dependencies or fine-grained details from balancedandcomprehensiverepresentation.
| theinputvideo. |     | Toaddressthisgap, |     |     | weapplyaselective- |     |     |     |     |     |     |
| -------------- | --- | ----------------- | --- | --- | ------------------ | --- | --- | --- | --- | --- | --- |
4.2.2.BidirectionalScan
| scan mechanism. |     | First, | we concatenate |     | the visual | queries |     |     |     |     |     |
| --------------- | --- | ------ | -------------- | --- | ---------- | ------- | --- | --- | --- | --- | --- |
with the spatiotemporal tokens Z, producing a combined The original selective scan developed for 1D sequences is
| tokensequenceZ′ |     | RL′×d,whereL′ |     |     |     |     |     |     |     |     |     |
| --------------- | --- | ------------- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
∈ = L+N. Wethen suboptimal for vision tasks as it lacks spatial awareness.
4

To address this limitation, we apply a bidirectional scan 5.3.OurModelVariants
(Figure2(c))[40],whichperformsaforward-and-backward
• BIMBA-LLaVA. We implement this variant by apply-
passthroughthevisualtokens. Thisforward-and-backward
ing BIMBA on the image-pretrained MLLM LLaVA-
scanthroughthevisualtokensenhancesthemodel’sability
NeXT [44], which uses CLIP [59] vision encoder and
tocapturespatiotemporalstructure,improvingitseffective-
Vicuna-7B[11]LLM.
nessforvideomodelingtasks.
• BIMBA-LLaMA. We implement this variant by apply-
ing BIMBA on the image-pretrained MLLM LLaMA-
4.2.3.Question-ConditionedTokenSelection
3.2[52],whichusesMeta-CLIP[74]visionencoderand
We also explore a variant of the token selector where we
LLaMA-3.2-LLM-8BLLM.
prepend the extracted tokens from the textual question to
thespatio-temporaltokensbeforepassingthemtothetoken 5.4.Baselines
selector. Thisvariantenhancesthemodel’sabilitytoselect
Inadditiontocomparingwithpriormethodsthatwerebased
themostrelevanttokensbytakingintoaccountthecontext
on different MLLMs and trained on different data, we im-
providedbythequestion. Theoperationisexpressedas:
plementedthefollowingbaselinesforafaircomparison.
• Vanilla. We implement this baseline by removing the
Z′ =[X;Z;Q], (6)
spatiotemporal token selector from our model, resulting
innotokencompression. Alltokensgeneratedbytheim-
where X represents the question tokenized by the LLM.
ageencoderarepasseddirectlytotheLLM.
Then, thecombinedtokensequenceZ′ ispassedtotheto-
• Pooling. Thisbaselineappliesspatiotemporalpoolingas
kenselectorasdescribedinSection4.2.
thecompressionmethod,usingthesamecompressionra-
tioasourmainmodel.
4.3.LLMDecoder
• Self-Attention. We implement this baseline by replac-
WeuseanLLMdecoderf togenerateatextualanswerR ing the selective-scan layer of our spatiotemporal token
L
from the concatenation of the tokenized input question X selectorwithself-attentionlayers.
andthesetofvisualqueriesQ′generatedbythespatiotem- • Perceiver. This baseline leverages the Perceiver [29]
poraltokenselector: mechanism to compress spatiotemporal tokens and uses
thesamecompressionratioasourmodel.
R=f ([X;Q′]). (7)
L
5.5.ImplementationDetails
5.ExperimentalSetup We use a simple strategy to train both our models and
baselines: starting from an image-pretrained frame en-
5.1.TrainingData
coder, we fine-tune the MLLM on the video instruction-
tuning dataset. We freeze the image encoder and train
For our default models, we trained our model on 370K
the spatiotemporal token selector and the LLM by ap-
instruction tuning videos aggregated from a collection of
plying LoRA [23] to the LLM. By default, the BIMBA-
datasets, including YouCook2 [87], Ego4D-HCap [28],
LLaVA model takes 64 video frames divided into 64 ×
NExT-QA [73], IntentQA [37], CLEVRER [78],
24×24 = 36,864tokensandcompressesthissequenceto
Ego4D [16], STAR [70], and Perception Test [57]. We
16×12×12=2,304tokenswhicharepassedtotheLLM.
did not use any data generated by ChatGPT or GPT-4, in
accordancewiththeOpenAItermsofuse1 andourinternal By default, BIMBA-LLaMA processes 64 frames divided
into64×40×40 = 102,400tokensandcompressesthis
legalpolicy. Inaddition,tospeedupablationexperiments,
sequenceto16×20×20=6,400tokensforLLM.
weconstructasmallerinstructiontuningtrainingsetof70K
videos from NExT-QA [73], IntentQA [37], Ego4D [16],
6.ResultsandAnalysis
and Ego4D-HCap [28]. The ablations are evaluated on
NExT-QA[73]andEgoSchema[51].
First, we analyze various aspects of our model, includ-
ing comparison with different compression methods (Sec-
5.2.EvaluationBenchmarks
tion 6.1), architectural design (Section 6.2), and question-
We evaluate our model on seven diverse video conditionedtokenselection(Section6.3). Lastly,wecom-
question-answering benchmarks: Perception Test [55], parewithstate-of-the-artvideoMLLMsinSection6.4.
NExT-QA [73], EgoSchema [51], VNBench [85],
6.1.ComparisonofCompressionMethods
LongVideoBench[71],Video-MME[15],andMLVU[86].
We compare BIMBA with other compression techniques
1https://openai.com/policies/row-terms-of-use andabaselinethatusesalltokenswithoutcompression.
5

74
58
72
| 70  |     |     |     |     |     |     | 55  |     |     |     |     |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
68
| ycaruccA |     |     |     |     |     |     | ycaruccA 52 |     |     |     |     |
| -------- | --- | --- | --- | --- | --- | --- | ----------- | --- | --- | --- | --- |
66
|     |     |     |     | BIMBA          |     |     | 49  |     |     | BIMBA          |     |
| --- | --- | --- | --- | -------------- | --- | --- | --- | --- | --- | -------------- | --- |
| 64  |     |     |     | Self-Attention |     |     |     |     |     | Self-Attention |     |
46
| 62  |     |         |        | Perceiver |     |     |     |            |        | Perceiver |         |
| --- | --- | ------- | ------ | --------- | --- | --- | --- | ---------- | ------ | --------- | ------- |
|     |     |         |        | Pooling   |     |     |     |            |        | Pooling   |         |
| 60  |     |         |        |           |     |     | 43  |            |        |           |         |
|     |     |         |        | Vanilla   |     |     |     |            |        | Vanilla   |         |
|     | 5K  | 10K 15K | 20K    | 25K 30K   | 35K |     |     | 5K 10K 15K | 20K    | 25K       | 30K 35K |
|     |     |         | Tokens |           |     |     |     |            | Tokens |           |         |
(a)NExT-QAperformanceofmodelsbasedonLLaVA. (b)EgoSchemaperformanceofmodelsbasedonLLaVA.
| 75  |     |     |     |     |     |     | 61  |     |     |     |     |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
58
73
| ycaruccA |     |     |     |     |     |     | ycaruccA |     |     |     |     |
| -------- | --- | --- | --- | --- | --- | --- | -------- | --- | --- | --- | --- |
55
71
|     |     |     |     | BIMBA |     |     |     |     |     | BIMBA |     |
| --- | --- | --- | --- | ----- | --- | --- | --- | --- | --- | ----- | --- |
52
|     |     |     |     | Self-Attention |     |     |     |     |     | Self-Attention |     |
| --- | --- | --- | --- | -------------- | --- | --- | --- | --- | --- | -------------- | --- |
69
|     |     |     |     | Perceiver |     |     |     |     |     | Perceiver |     |
| --- | --- | --- | --- | --------- | --- | --- | --- | --- | --- | --------- | --- |
49
|     |     |     |     | Pooling |     |     |     |     |     | Pooling |     |
| --- | --- | --- | --- | ------- | --- | --- | --- | --- | --- | ------- | --- |
| 67  |     |     |     | Vanilla |     |     |     |     |     | Vanilla |     |
46
|     | 20K | 40K | 60K    | 80K | 100K |     |     | 20K 40K | 60K    | 80K | 100K |
| --- | --- | --- | ------ | --- | ---- | --- | --- | ------- | ------ | --- | ---- |
|     |     |     | Tokens |     |      |     |     |         | Tokens |     |      |
(c)NExT-QAperformanceofmodelsbasedonLLaMA. (d)EgoSchemaperformanceofmodelsbasedontheLLaMA.
Figure3. AccuracyachievedbyBIMBAandbaselinemodelsonNeXT-QA(left)andEgoSchema(right)asafunctionofthenumberof
inputtokensformodelsbasedonLLaVA(toprow)andLLaMA(bottomrow). BIMBAachievesthehighestaccuracyforallsequence
lengths,andthedifferencewithotherbaselinesincreasesasweincreasethenumberofinputtokens. Self-attentioncannotbeappliedto
longsequencesasitcausesGPUout-of-memoryissuesoncethenumberoftokensbecomestoolarge.
Analysis of accuracy. We begin by analyzing the accu- indicating that the pooling compression strategy struggles
racy of the different models as a function of the number to capture long-range spatiotemporal dependencies when
of input spatiotemporal tokens. Specifically, we vary the manyframesaregivenasinput. Third,whileself-attention
numberofframessampledfromthevideotoproduceinput performssimilarlytoBIMBAforshortinputsequences(up
token sequences of varying lengths. We experiment with to 16 frames or 9,216 tokens for BIMBA-LLaVA and 8
1, 4, 8, 16, 32, and 64 frames, resulting in 576, 2,304, framesor12,800tokensforBIMBA-LLaMA),itrunsoutof
9,216, 18,432, and 36,864 input tokens for models based GPUmemorybeyondthat. Incomparison,ourmodelcon-
on LLaVA, and 1,600, 6,400, 12,800, 25,600, 51,200, and sistentlyoutperformsthewidelyusedcompressionmecha-
102,400 input tokens for models based on LLaMA. While nism Perceiver [29] and other methods, achieving consis-
the Vanilla method generates a number of output tokens tently higher accuracy for all token sequence lengths with
equaltothelengthoftheinputsequence,weapplythecom- a monotonically increasing performance as more frames
pressionmethodsinthiscomparisonusingcompressionof are added to the input. It yields the highest performance
1×2×2inspatiotemporal(T×H×W)dimensionsupto at 36,864 tokens for BIMBA-LLaVA and 102,400 tokens
16framesand2×2×2forinputsof32framesand4×2×2 for BIMBA-LLaMA. This demonstrates the effectiveness
forsequencesof64frames. ofourtokencompressionmechanismforlong-rangevideo
understanding.
| Figure | 3 shows         | accuracy |        | for LLaVA-based |     | models (top   |     |     |     |     |     |
| ------ | --------------- | -------- | ------ | --------------- | --- | ------------- | --- | --- | --- | --- | --- |
| row)   | and LLaMA-based |          | models | (bottom         |     | row) on NeXT- |     |     |     |     |     |
QA (left) and EgoSchema (right). First, our results show Analysisofcomputationalcost. Inthissection,wecom-
parethecomputationalcostofourmodelagainstbaselines
| that the | Vanilla | method | performs | significantly |     | worse than |     |     |     |     |     |
| -------- | ------- | ------ | -------- | ------------- | --- | ---------- | --- | --- | --- | --- | --- |
ourBIMBA,underscoringtheimportanceofspatiotemporal intermsofGPUmemoryusage(Figure4(left))andruntime
token compression for video question-answering. Second, (Figure4(right)). WereportresultsforLLaVA-basedmod-
we observe that as the number of input tokens increases, els, while the supplementary material provides an analysis
theperformanceofthepoolingmethodsaturates. Forpool- ofLLaMA-basedmodels.
ing models, performance even declines beyond 16 frames, As shown in Figure 4 (left), both the Vanilla method
6

60
|     |     |     |     | BIMBA          |     |     |     |     | 4   |     |     | BIMBA          |     |     |
| --- | --- | --- | --- | -------------- | --- | --- | --- | --- | --- | --- | --- | -------------- | --- | --- |
|     |     |     |     | Self-Attention |     |     |     |     |     |     |     | Self-Attention |     |     |
|     |     |     |     | Perceiver      |     |     |     |     |     |     |     | Perceiver      |     |     |
3
| )BG(yromeM |     |     |     | Pooling |     |     |     |     | )ces(emitnuR |     |     | Pooling |     |     |
| ---------- | --- | --- | --- | ------- | --- | --- | --- | --- | ------------ | --- | --- | ------- | --- | --- |
40
|     |     |     |     | Vanilla |     |     |     |     |     |     |     | Vanilla |     |     |
| --- | --- | --- | --- | ------- | --- | --- | --- | --- | --- | --- | --- | ------- | --- | --- |
2
1
20
0
|     | 5K  | 10K 15K | 20K    | 25K 30K | 35K |     |     |     |     | 5K 10K 15K | 20K    | 25K | 30K 35K |     |
| --- | --- | ------- | ------ | ------- | --- | --- | --- | --- | --- | ---------- | ------ | --- | ------- | --- |
|     |     |         | Tokens |         |     |     |     |     |     |            | Tokens |     |         |     |
(a)MemoryUsageofModelsbasedonLLaVA. (b)RuntimeofmodelsbasedonLLaVA.
Figure4. ComputationcostofBIMBAandbaselinemodelsintermsofmemoryusage(left)andruntime(right). Allmodelsarebasedon
LLaVA.Modelsbasedonself-attentionorthatdonotperformcompression(Vanilla)runquicklyoutofmemoryasthenumberofinput
tokensisincreased.TheruntimeofBIMBAgrowsgracefullyasafunctionoftheinputsequencelength,unlikeforthecaseofVanilla.
QueryType Norm Bidirectional Interleaved LLaVA LLaMA tional selective scan improves accuracy over the standard
Learnable 67.67 68.91 selective scan by +1.83% and +2.07%, validating our hy-
Average 68.61 70.56 pothesis that the standard selective scan, originally devel-
Average ✓ 69.85 71.16 oped for NLP, is suboptimal for vision tasks. Finally, in-
|         |     | ✓   | ✓   |     |     |       |       |                                                      |             |      |              |          |         |     |
| ------- | --- | --- | --- | --- | --- | ----- | ----- | ---------------------------------------------------- | ----------- | ---- | ------------ | -------- | ------- | --- |
| Average |     |     |     |     |     | 71.68 | 73.23 |                                                      |             |      |              |          |         |     |
|         |     | ✓   | ✓   |     | ✓   |       |       | terleavingqueriesamongspatiotemporaltokensyieldsbet- |             |      |              |          |         |     |
| Average |     |     |     |     |     | 73.57 | 75.57 |                                                      |             |      |              |          |         |     |
|         |     |     |     |     |     |       |       | ter                                                  | performance | than | the standard | approach | (+1.89% | and |
Table1. Ablationsshowingtheeffectsofourdesignchoiceson
+2.34%)sincethisdesignallowsthemodeltocapturelong-
NExT-QA.Averagepoolinginitializationofthequeries,layernor-
rangetemporaldependenciesinvideodatamoreeffectively.
malizationbeforetokenselection,bidirectionalselectivescan,and
queryinterleavingelevatesignificantlytheaccuracyofBIMBA.
6.3.Question-ConditionedTokenSelection
| and self-attention |     | exhibit | a   | sharp increase | in  | memory | us- |     |     |     |     |     |     |     |
| ------------------ | --- | ------- | --- | -------------- | --- | ------ | --- | --- | --- | --- | --- | --- | --- | --- |
ageastheinputlengthincreases,leadingtoout-of-memory
|       |        |               |     |        |         |        |        |     | Question | BIMBA-LLaVA |     |     | BIMBA-LLaMA |     |
| ----- | ------ | ------------- | --- | ------ | ------- | ------ | ------ | --- | -------- | ----------- | --- | --- | ----------- | --- |
| (OOM) | errors | for sequences |     | longer | than 16 | frames | (9,216 |     |          |             |     |     |             |     |
Conditioning
|                                                |            |           |     |            |            |        |         |     |     | NextQA | EgoSchema |     | NextQA EgoSchema |       |
| ---------------------------------------------- | ---------- | --------- | --- | ---------- | ---------- | ------ | ------- | --- | --- | ------ | --------- | --- | ---------------- | ----- |
| tokens).                                       | Pooling    | consumes  |     | the least  | GPU memory |        | but, as |     |     |        |           |     |                  |       |
|                                                |            |           |     |            |            |        |         |     | ✗   | 73.57  | 58.40     |     | 75.57            | 62.20 |
| previouslyobserved,resultsinthelowestaccuracy. |            |           |     |            |            |        | Incon-  |     |     |        |           |     |                  |       |
|                                                |            |           |     |            |            |        |         |     | ✓   | 74.77  | 60.51     |     | 76.61            | 64.33 |
| trast,                                         | our method | maintains |     | low memory | usage      | across | all     |     |     |        |           |     |                  |       |
inputlengthswhileachievingthehighestaccuracy. Table2. QuestionconditioningimprovestheaccuracyofBIMBA
Figure 4 (right) presents runtime comparisons. We can onbothNExT-QAandEgoSchema.
observe that the runtime of the Vanilla method grows dra- Thissectionanalyzestheimpactofconditioningourspa-
maticallywiththeinputlength. Self-attentionalsoincursa tiotemporaltokenselectorwithinformationextractedfrom
higher runtime cost, and we can only experiment with se- thetextualquestion. AsshowninTable2,addingtheques-
quences up to 16 frames or 9,216 tokens. Although pool- tion as input to the token selector improves the accuracy
ingandPerceivermaintainlowmemoryandruntimecosts by 1.04% on NExT-QA and by 2.13% on EgoSchema for
similartoourmethod,theiraccuracyissignificantlylower. the BIMBA-LLaMA model. We also observe similar im-
Finally,ourmethodhasaruntimecostnearlyaslowaspool- provements for adding question-conditioning to BIMBA-
ingandcanefficientlyprocesssequencesupto64framesor LLaVAT˙hisenhancementdemonstratesthatincludingques-
36,864tokens. tion tokens enables our selective-scan mechanism to focus
oncontentspecificallyrelatedtothequestion,allowingitto
6.2.AblationonArchitectureDesign
choosemorerelevantspatiotemporaltokensfromtheinput
Inthissection,weanalyzeseveralkeyarchitecturaldesign video. We do not include this question-conditioned token
selectiontechniqueinourmainBIMBAmodelbecausewe
| choices | that | contribute | to the | effectiveness |     | of our | model. |     |     |     |     |     |     |     |
| ------- | ---- | ---------- | ------ | ------------- | --- | ------ | ------ | --- | --- | --- | --- | --- | --- | --- |
WepresenttheresultsontheNExT-QAdatasetinTable1. expectBIMBAtoanswermultiplequestionsaboutavideo
First, we observe that initializing visual queries using a efficientlybasedononesequenceofselectedtokenswithout
3Dpoolinglayer, asdescribedinSection4.2, outperforms recomputingthemforeachquestion.
| random | initialization, |     | yielding | accuracy | improvements |     | of  |     |     |     |     |     |     |     |
| ------ | --------------- | --- | -------- | -------- | ------------ | --- | --- | --- | --- | --- | --- | --- | --- | --- |
6.4.ComparisonwiththeStateoftheArt
| +0.94% | and | +1.65% | for | BIMBA-LLaVA |     | and BIMBA- |     |     |     |     |     |     |     |     |
| ------ | --- | ------ | --- | ----------- | --- | ---------- | --- | --- | --- | --- | --- | --- | --- | --- |
LLaMA,respectively. Second,incorporatinglayernormal- Inthissection,wecompareourmodelagainststate-of-the-
ization before the spatiotemporal token selector enhances art video MLLMs on seven different video benchmarks,
performance(seerow2vs. row3). Third,usingabidirec- summarizing the results in Table 3. Since prior models
7

Model LLM Frames PerceptionTest NExT-QA EgoSchema VNBench LongVideoBench Video-MME MLVU
Duration 23sec 44sec 180sec 180sec 8sec-60min 1-60min 3-120min
(BasedonProprietaryLLMs)
| LLoVi[79]      |     | GPT3.5 | -   | -   | 66.30 | 52.00 | -   | -   | -   | -   |
| -------------- | --- | ------ | --- | --- | ----- | ----- | --- | --- | --- | --- |
| VideoAgent[67] |     | GPT4   | -   | -   | 71.30 | 54.10 | -   | -   | -   | -   |
| VideoTree[69]  |     | GPT4   | -   | -   | 75.60 | 61.10 | -   | -   | -   | -   |
(Open-sourceMLLMsTrainedonOur370KVideoInstructionData)
LLaVA-NeXT(Video)[44] Vicuna-7B 64 46.13 67.66 41.66 - - 42.21 42.33
| PLLaVA[75]  |     | Vicuna-7B | 64  | 48.55 | 67.56 | 43.36 | -   | -   | 42.13 | 44.61 |
| ----------- | --- | --------- | --- | ----- | ----- | ----- | --- | --- | ----- | ----- |
| BIMBA-LLaVA |     | Vicuna-7B | 64  | 52.61 | 72.35 | 52.31 | -   | -   | 45.66 | 47.16 |
LLaMA-3.2(Video)[52] LLaMA3.2-8B 64 53.16 73.72 55.21 - - 47.31 47.61
PLLaVA(LLaMA-3.2)[52] LLaMA3.2-8B 64 53.13 72.77 52.33 - - 46.56 46.56
| BIMBA-LLaMA |     | LLaMA3.2-8B | 64  | 56.50 | 76.88 | 60.25 | -   | -   | 50.11 | 50.60 |
| ----------- | --- | ----------- | --- | ----- | ----- | ----- | --- | --- | ----- | ----- |
(Open-sourceMLLMsTrainedonLargerScaleInstructionVideoData)
| LLaMA-VID[41] |     | Vicuna-7B | 1fps | 44.60 | -   | 38.50 | 10.80 | -   | -   | 33.20 |
| ------------- | --- | --------- | ---- | ----- | --- | ----- | ----- | --- | --- | ----- |
Video-LLaVA[42] Vicuna-7B 8 44.30 62.60 38.40 - - 40.40 47.30
LLaVA-NeXT-Video[82] Vicuna-7B 32 48.80 - 43.90 20.10 43.50 46.50 -
| PLLaVA[75] |     | Vicuna-7B | 16  | -   | 68.17 | 45.16 | -   | 40.20 | 44.25 | 47.31 |
| ---------- | --- | --------- | --- | --- | ----- | ----- | --- | ----- | ----- | ----- |
VideoChat2[39] Vicuna-7B 16 47.30 - 54.40 12.40 36.00 47.90 54.60
| LongVA[81] |     | Qwen2-7B | 128 | -   | 68.30 | -   | 41.50 | -   | 54.30 | 56.30 |
| ---------- | --- | -------- | --- | --- | ----- | --- | ----- | --- | ----- | ----- |
Video-LLaMA2[10] Qwen2-7B 32 51.40 - 51.70 24.90 - 47.90 48.50
LLaVA-OneVision[34] Qwen2-7B 32 57.10 79.40 60.10 51.80 56.5 58.20 64.70
| Kangaroo[47] |     | LLaMA2-8B | 64   | -   | -   | 62.70 | -     | -     | 56.00 | 61.00 |
| ------------ | --- | --------- | ---- | --- | --- | ----- | ----- | ----- | ----- | ----- |
| Video-XL[62] |     | Qwen2-7B  | 2048 | -   | -   | -     | 61.60 | 49.50 | 55.50 | 64.90 |
| LongVU[61]   |     | Qwen2-7B  | 1fps | -   | -   | 67.60 | -     | -     | 60.60 | 65.40 |
| Qwen2-VL[66] |     | Qwen2-7B  | 2fps | -   | -   | -     | -     | 55.60 | 63.30 | -     |
LLaVA-Video[83] Qwen2-7B 64 67.90 83.20 57.30 70.77 58.20 63.30 70.80
BIMBA-LLaVA Qwen2-7B 128 68.51 83.73 71.14 77.88 59.46 64.67 71.37
Table3. WecompareBIMBAwithstate-of-the-artvideoMLLMsacrosssevendiversevideoquestion-answeringbenchmarks. BIMBA-
LLaVAachievesthehighestperformanceonalldatasetswhenusingtheQwen2-7BLLMbackbone(thirdsection).SincedifferentMLLMs
leverage varying LLM backbones and training data, we also conduct a fair comparison by evaluating our model against four baselines
trainedusingthesame370Kinstruction-tuningdatasetandusingVicuna-7BandLLaMA3.2-8BLLMdecoders(secondsection). Inthis
evaluationsetting,weobservethatourmodelconsistentlyoutperformsallbaselinesacrossallbenchmarks.
rely on varying backbones and training datasets, to en- state-of-the-artperformanceonmultiplechallengingvideo
sure a fair comparison, we first fine-tune four competi- question-answeringbenchmarks,includingNExT-QA[73],
tive models, as well as the two default variants of our EgoSchema [51], VNBench [85], LongVideoBench [71],
BIMBA model on our 370K instruction-tuning dataset. andVideo-MME[15]. OntheEgoSchemabenchmark,our
These include PLLaVA [75], LLaVA-NeXT (Video) [44], model surpasses the previous best method, LongVU [61],
LLaMA-3.2 [52] (Video) and PLLaVA (LLaMA-3.2), a by 3.54%, demonstrating its superior ability to compre-
variantofPLLaVAusingthestrongerLLaMA-3.2[52]im- hend egocentric videos and handle questions that require
age MLLM. The results in the second section of the ta- longcontextunderstanding. Similarly,onVNBench,which
ble show that our model achieves better results than all focuses on needle-in-the-haystack questions, our approach
these baselines. In particular, we note that when using the outperformsLLaVA-Video[83]by7.11%, highlightingits
sameVicuna-7B[11]model, BIMBA-LLaVAoutperforms strongcapabilitytoextractkeyinformationfromverylong
both PLLaVA and LLaVA-NeXT (Video) on all bench- videos. Additionally, BIMBA-LLaVA (Qwen2-7B) sub-
marks,achievinganaverageaccuracyof55.23%compared stantiallyoutperformsmodelsbasedonproprietaryLMMs.
to 49.42% for LLaVA-NeXT and 50.40% for PLLaVA. Specifically, it surpasses VideoTree [69] (GPT-4) by 7.7%
Likewise, ourBIMBA-LLaMAvariantsurpassesLLaMA- on NExT-QA and 10.04% on EgoSchema. Finally, on
3.2 (Video) by 3.09% on average and PLLaVA (LLaMA- benchmarks requiring long video comprehension, such as
3.2) by 4.21%. These results highlight the effectiveness LongVideoBench[71], Video-MME[15], andMLVU[86]
ofourselective-scan-basedtokencompressionmechanism, ourmodelsetsanewstate-of-the-art,furtherdemonstrating
which can be seamlessly integrated into existing MLLMs itseffectivenessinprocessingandunderstandinghour-long
| to enhance performance |     | across a | wide range | of video | un- | videos. |     |     |     |     |
| ---------------------- | --- | -------- | ---------- | -------- | --- | ------- | --- | --- | --- | --- |
derstandingtasks.
7.Conclusion
| Furthermore, | since | recent MLLMs | [34, 47, | 61, 62, | 66, |     |     |     |     |     |
| ------------ | ----- | ------------ | -------- | ------- | --- | --- | --- | --- | --- | --- |
83] have been trained with significantly larger instruction- We introduced BIMBA, an efficient multimodal large lan-
tuning datasets and stronger LLM backbones, we also guage model for video question answering. By using
trained a variant of BIMBA-LLaVA using approximately a spatiotemporal token selector based on selective scan,
1.6 million video instruction samples and the Qwen2- BIMBA dramatically compresses sequences of tokens ex-
7B LLM backbone. The results in the third section of tracted from long-form video while preserving salient in-
Table 3 show that this BIMBA-LLaVA variant achieves formation, achieving significant memory and runtime sav-
8

ingscomparedtoexistingapproaches. Weintroducesimple els and aligning for generic visual-linguistic tasks. arXiv
buteffectivedesigncontributions, includinganinterleaved preprintarXiv:2312.14238,2023. 1
token arrangement to mitigate positional bias and a bidi- [10] Zesen Cheng, Sicong Leng, Hang Zhang, Yifei Xin, Xin
rectionalselective-scanmechanismforbettermodelingthe Li, Guanzheng Chen, Yongxin Zhu, Wenqi Zhang, Ziyang
spatiotemporalstructureofthevideo. Experimentalresults Luo, Deli Zhao, et al. Videollama 2: Advancing spatial-
|                   |           |                     |                  |      |             |         | temporal                            | modeling |         | and audio | understanding |           | in     | video-llms. |
| ----------------- | --------- | ------------------- | ---------------- | ---- | ----------- | ------- | ----------------------------------- | -------- | ------- | --------- | ------------- | --------- | ------ | ----------- |
| show that         | our model | achieves            | state-of-the-art |      | performance |         |                                     |          |         |           |               |           |        |             |
|                   |           |                     |                  |      |             |         | arXivpreprintarXiv:2406.07476,2024. |          |         |           |               |           | 2,8    |             |
| on multiple       | datasets, | marking             | a promising      |      | advance     | for ef- |                                     |          |         |           |               |           |        |             |
|                   |           |                     |                  |      |             |         | [11] Wei-Lin                        | Chiang,  | Zhuohan |           | Li, Zi        | Lin, Ying | Sheng, | Zhang-      |
| ficient long-form |           | video understanding |                  | with | LLMs.       | In the  |                                     |          |         |           |               |           |        |             |
haoWu,HaoZhang,LianminZheng,SiyuanZhuang,Yong-
future, weareinterestedinexploringtheadaptationofour
|     |     |     |     |     |     |     | hao | Zhuang, | Joseph | E. Gonzalez, |     | Ion | Stoica, | and Eric P. |
| --- | --- | --- | --- | --- | --- | --- | --- | ------- | ------ | ------------ | --- | --- | ------- | ----------- |
modeltoothertasks,suchasvideosummarization,text-to-
|     |     |     |     |     |     |     | Xing. | Vicuna: | An  | open-source |     | chatbot | impressing | gpt-4 |
| --- | --- | --- | --- | --- | --- | --- | ----- | ------- | --- | ----------- | --- | ------- | ---------- | ----- |
videosearch,andhierarchicalvideomodeling.
|                   |     |     |              |      |      |        | with90%*chatgptquality,2023. |       |     |        |        | 1,5,8,13 |        |      |
| ----------------- | --- | --- | ------------ | ---- | ---- | ------ | ---------------------------- | ----- | --- | ------ | ------ | -------- | ------ | ---- |
| Acknowledgements. |     | We  | thank Yan-Bo | Lin, | Feng | Cheng, |                              |       |     |        |        |          |        |      |
|                   |     |     |              |      |      |        | [12] Xiaoyi                  | Dong, | Pan | Zhang, | Yuhang | Zang,    | Yuhang | Cao, |
Ce Zhang, Yue Yang, Yulu Pan, Han Yi, Triantafyllos Bin Wang, Linke Ouyang, Xilin Wei, Songyang Zhang,
Afouras, and Effrosyni Mavroudi for their helpful discus- HaodongDuan,MaosongCao,etal. Internlm-xcomposer2:
| sions. This | work | was supported |     | by the | Laboratory | for |           |     |           |            |     |             |     |         |
| ----------- | ---- | ------------- | --- | ------ | ---------- | --- | --------- | --- | --------- | ---------- | --- | ----------- | --- | ------- |
|             |      |               |     |        |            |     | Mastering |     | free-form | text-image |     | composition | and | compre- |
Analytic Sciences via NC State University, ONR Award hension in vision-language large model. arXiv preprint
| N00014-23-1-2356. |     |     |     |     |     |     | arXiv:2401.16420,2024. |              |     | 1   |       |          |       |        |
| ----------------- | --- | --- | --- | --- | --- | --- | ---------------------- | ------------ | --- | --- | ----- | -------- | ----- | ------ |
|                   |     |     |     |     |     |     | [13] Alexey            | Dosovitskiy. |     | An  | image | is worth | 16x16 | words: |
References Transformersforimagerecognitionatscale. arXivpreprint
|     |     |     |     |     |     |     | arXiv:2010.11929,2020. |     |     | 3   |     |     |     |     |
| --- | --- | --- | --- | --- | --- | --- | ---------------------- | --- | --- | --- | --- | --- | --- | --- |
[1] Josh Achiam, Steven Adler, Sandhini Agarwal, Lama Ah- [14] Abhimanyu Dubey, Abhinav Jauhri, Abhinav Pandey, Ab-
mad,IlgeAkkaya,FlorenciaLeoniAleman,DiogoAlmeida, hishek Kadian, Ahmad Al-Dahle, Aiesha Letman, Akhil
| JankoAltenschmidt,        |           | SamAltman, |                | ShyamalAnadkat, |                   | etal.   |                                             |       |           |       |          |     |                   |     |
| ------------------------- | --------- | ---------- | -------------- | --------------- | ----------------- | ------- | ------------------------------------------- | ----- | --------- | ----- | -------- | --- | ----------------- | --- |
|                           |           |            |                |                 |                   |         | Mathur,AlanSchelten,AmyYang,AngelaFan,etal. |       |           |       |          |     |                   | The |
| Gpt-4                     | technical | report.    | arXiv preprint |                 | arXiv:2303.08774, |         |                                             |       |           |       |          |     |                   |     |
|                           |           |            |                |                 |                   |         | llama                                       | 3herd | ofmodels. | arXiv | preprint |     | arXiv:2407.21783, |     |
| 2023.                     | 1         |            |                |                 |                   |         | 2024.                                       | 1     |           |       |          |     |                   |     |
| [2] Jean-BaptisteAlayrac, |           |            | JeffDonahue,   | PaulineLuc,     |                   | Antoine |                                             |       |           |       |          |     |                   |     |
[15] ChaoyouFu,YuhanDai,YondongLuo,LeiLi,ShuhuaiRen,
| Miech, | Iain | Barr, Yana | Hasson, | Karel Lenc, | Arthur | Men- |        |        |       |       |        |       |         |       |
| ------ | ---- | ---------- | ------- | ----------- | ------ | ---- | ------ | ------ | ----- | ----- | ------ | ----- | ------- | ----- |
|        |      |            |         |             |        |      | Renrui | Zhang, | Zihan | Wang, | Chenyu | Zhou, | Yunhang | Shen, |
sch,KatherineMillican,MalcolmReynolds,etal.Flamingo: MengdanZhang,etal. Video-mme: Thefirst-evercompre-
a visual language model for few-shot learning. Advances hensiveevaluationbenchmarkofmulti-modalllmsinvideo
inneuralinformationprocessingsystems,35:23716–23736, analysis. arXivpreprintarXiv:2405.21075,2024. 2,5,8
| 2022. | 1   |     |     |     |     |     |              |          |     |        |           |     |        |        |
| ----- | --- | --- | --- | --- | --- | --- | ------------ | -------- | --- | ------ | --------- | --- | ------ | ------ |
|       |     |     |     |     |     |     | [16] Kristen | Grauman, |     | Andrew | Westbury, |     | Eugene | Byrne, |
[3] KirolosAtaallah, XiaoqianShen, EslamAbdelrahman, Es- Zachary Chavis, Antonino Furnari, Rohit Girdhar, Jack-
sam Sleiman, Deyao Zhu, Jian Ding, and Mohamed El- son Hamburger, Hao Jiang, Miao Liu, and Xingyu Liu et
hoseiny. Minigpt4-video: Advancing multimodal llms for al. Ego4d: Around the world in 3,000 hours of egocen-
video understanding with interleaved visual-textual tokens. tricvideo. IEEEConf.Comput.Vis.PatternRecog., pages
| arXivpreprintarXiv:2404.03413,2024. |     |     |     |     | 1,2 |     |                   |     |     |     |     |     |     |     |
| ----------------------------------- | --- | --- | --- | --- | --- | --- | ----------------- | --- | --- | --- | --- | --- | --- | --- |
|                                     |     |     |     |     |     |     | 18995–19012,2022. |     |     | 5   |     |     |     |     |
[4] Jinze Bai, Shuai Bai, Shusheng Yang, Shijie Wang, Sinan [17] Albert Gu and Tri Dao. Mamba: Linear-time sequence
Tan, Peng Wang, Junyang Lin, Chang Zhou, and Jingren modeling with selective state spaces. arXiv preprint
Zhou.Qwen-vl:Afrontierlargevision-languagemodelwith arXiv:2312.00752,2023.
1,2,3,4
versatileabilities.arXivpreprintarXiv:2308.12966,2023.1
[18] AlbertGu,TriDao,StefanoErmon,AtriRudra,andChristo-
[5] Tom B Brown. Language models are few-shot learners. pherRe´.Hippo:Recurrentmemorywithoptimalpolynomial
arXivpreprintarXiv:2005.14165,2020. 1 projections.AdvancesinNeuralInformationProcessingSys-
| [6] SoumyabrataChaudhuriandSaumikBhattacharya. |     |     |     |     |     | Simba: | tems,33,2020. |     |     |     |     |     |     |     |
| ---------------------------------------------- | --- | --- | --- | --- | --- | ------ | ------------- | --- | --- | --- | --- | --- | --- | --- |
1,2
Mambaaugmentedu-shiftgcnforskeletalactionrecognition [19] AlbertGu,IsysJohnson,KaranGoel,KhaledSaab,TriDao,
invideos. arXivpreprintarXiv:2404.07645,2024. 2 AtriRudra,andChristopherRe´. Combiningrecurrent,con-
[7] Guo Chen, Yin-Dong Zheng, Jiahao Wang, Jilan Xu, Yifei volutional, and continuous-time models with linear state-
Huang, Junting Pan, Yi Wang, Yali Wang, Yu Qiao, Tong space layers. Advances in Neural Information Processing
| Lu, | et al. | Videollm: Modeling |     | video sequence |     | with large | Systems,34,2021. |     |     |     |     |     |     |     |
| --- | ------ | ------------------ | --- | -------------- | --- | ---------- | ---------------- | --- | --- | --- | --- | --- | --- | --- |
languagemodels.arXivpreprintarXiv:2305.13292,2023.1 [20] AlbertGu,KaranGoel,andChristopherRe´.Efficientlymod-
[8] Keyan Chen, Bowen Chen, Chenyang Liu, Wenyuan Li, elinglongsequenceswithstructuredstatespaces. InTheIn-
ZhengxiaZou,andZhenweiShi. Rsmamba: Remotesens- ternationalConferenceonLearningRepresentations(ICLR),
| ingimageclassificationwithstatespacemodel. |     |     |     |     |     | IEEEGeo- | 2022. | 2,3 |     |     |     |     |     |     |
| ------------------------------------------ | --- | --- | --- | --- | --- | -------- | ----- | --- | --- | --- | --- | --- | --- | --- |
scienceandRemoteSensingLetters,2024. 2 [21] Albert Gu, Ankit Gupta, Karan Goel, and Christopher Re´.
[9] ZheChen,JiannanWu,WenhaiWang,WeijieSu,GuoChen, On the parameterization and initialization of diagonal state
Sen Xing, Zhong Muyan, Qinglong Zhang, Xizhou Zhu, spacemodels. AdvancesinNeuralInformationProcessing
| LeweiLu,etal.Internvl:Scalingupvisionfoundationmod- |     |     |     |     |     |     | Systems,35,2022. |     |     |     |     |     |     |     |
| --------------------------------------------------- | --- | --- | --- | --- | --- | --- | ---------------- | --- | --- | --- | --- | --- | --- | --- |
9

[22] AnkitGupta,AlbertGu,andJonathanBerant.Diagonalstate [36] Junnan Li, Dongxu Li, Silvio Savarese, and Steven Hoi.
spacesareaseffectiveasstructuredstatespaces. Advances Blip-2: Bootstrapping language-image pre-training with
inNeuralInformationProcessingSystems,35:22982–22994, frozen image encoders and large language models. In In-
2022. 1,2 ternational conference on machine learning, pages 19730–
[23] Edward J Hu, Yelong Shen, Phillip Wallis, Zeyuan Allen- 19742.PMLR,2023. 1
Zhu,YuanzhiLi,SheanWang,LuWang,andWeizhuChen. [37] JiapengLi,PingWei,WenjuanHan,andLifengFan. Inten-
Lora: Low-rankadaptationoflargelanguagemodels. arXiv InProceedings
tqa: Context-awarevideointentreasoning.
preprintarXiv:2106.09685,2021. 5,13 oftheIEEE/CVFInternationalConferenceonComputerVi-
[24] De-An Huang, Shijia Liao, Subhashree Radhakrishnan, sion,pages11963–11974,2023. 5
HongxuYin,PavloMolchanov,ZhidingYu,andJanKautz. [38] KunChang Li, Yinan He, Yi Wang, Yizhuo Li, Wenhai
| Lita: | Language | instructed |     | temporal-localization |     |     | assistant. |       |      |      |            |       |           |          |
| ----- | -------- | ---------- | --- | --------------------- | --- | --- | ---------- | ----- | ---- | ---- | ---------- | ----- | --------- | -------- |
|       |          |            |     |                       |     |     |            | Wang, | Ping | Luo, | Yali Wang, | Limin | Wang, and | Yu Qiao. |
arXivpreprintarXiv:2403.19046,2024. 1 Videochat:Chat-centricvideounderstanding.arXivpreprint
[25] WeiquanHuang,YifeiShen,andYifanYang. Clip-mamba: arXiv:2305.06355,2023. 1,2
Clippretrainedmambamodelswithoodandhessianevalua-
|       |                                     |     |     |     |     |     |     | [39] Kunchang |     | Li, Yali | Wang, | Yinan He, | Yizhuo Li, | Yi Wang,    |
| ----- | ----------------------------------- | --- | --- | --- | --- | --- | --- | ------------- | --- | -------- | ----- | --------- | ---------- | ----------- |
| tion. | arXivpreprintarXiv:2404.19394,2024. |     |     |     |     | 2   |     |               |     |          |       |           |            |             |
|       |                                     |     |     |     |     |     |     | Yi Liu,       | Zun | Wang,    | Jilan | Xu, Guo   | Chen, Ping | Luo, et al. |
[26] MdMohaiminulIslamandGedasBertasius.Longmovieclip Mvbench: Acomprehensivemulti-modalvideounderstand-
classification with state-space video models. In European ingbenchmark. ArXivabs/2311.17005,2023. 1,8
| Conference |     | on Computer |     | Vision, pages | 87–104. |     | Springer, |               |     |            |     |          |           |            |
| ---------- | --- | ----------- | --- | ------------- | ------- | --- | --------- | ------------- | --- | ---------- | --- | -------- | --------- | ---------- |
|            |     |             |     |               |         |     |           | [40] Kunchang |     | Li, Xinhao | Li, | Yi Wang, | Yinan He, | Yali Wang, |
| 2022.      | 2   |             |     |               |         |     |           |               |     |            |     |          |           |            |
LiminWang,andYuQiao.Videomamba:Statespacemodel
[27] MdMohaiminulIslam,MahmudulHasan,KishanShamsun- forefficientvideounderstanding. InEuropeanConference
dar Athrey, Tony Braskich, and Gedas Bertasius. Efficient onComputerVision,pages237–255.Springer,2025. 2,5
| movie | scene | detection | using | state-space | transformers. |     | In  |                                         |     |     |     |     |            |     |
| ----- | ----- | --------- | ----- | ----------- | ------------- | --- | --- | --------------------------------------- | --- | --- | --- | --- | ---------- | --- |
|       |       |           |       |             |               |     |     | [41] YanweiLi,ChengyaoWang,andJiayaJia. |     |     |     |     | Llama-vid: | An  |
ProceedingsoftheIEEE/CVFConferenceonComputerVi-
|     |     |     |     |     |     |     |     | image | is worth | 2 tokens | in  | large language | models. | ArXiv |
| --- | --- | --- | --- | --- | --- | --- | --- | ----- | -------- | -------- | --- | -------------- | ------- | ----- |
sionandPatternRecognition,pages18749–18758,2023. 2 abs/2311.17043,2023. 2,8
[28] MdMohaiminulIslam,NganHo,XitongYang,TusharNa-
|                                             |                                       |     |     |     |     |            |     | [42] Bin | Lin, Bin | Zhu,         | Yang Ye, | Munan       | Ning, Peng    | Jin, and  |
| ------------------------------------------- | ------------------------------------- | --- | --- | --- | --- | ---------- | --- | -------- | -------- | ------------ | -------- | ----------- | ------------- | --------- |
| garajan,LorenzoTorresani,andGedasBertasius. |                                       |     |     |     |     | Videore-   |     |          |          |              |          |             |               |           |
|                                             |                                       |     |     |     |     |            |     | Li Yuan. |          | Video-llava: | Learning |             | united visual | represen- |
| cap:                                        | Recursivecaptioningofhour-longvideos. |     |     |     |     | InProceed- |     |          |          |              |          |             |               |           |
|                                             |                                       |     |     |     |     |            |     | tation   | by       | alignment    | before   | projection. | arXiv         | preprint  |
ingsoftheIEEE/CVFConferenceonComputerVisionand arXiv:2311.10122,2023. 1,8
| PatternRecognition,pages18198–18208,2024. |            |              |          |            |            | 5             |         |              |                                               |          |     |            |         |          |
| ----------------------------------------- | ---------- | ------------ | -------- | ---------- | ---------- | ------------- | ------- | ------------ | --------------------------------------------- | -------- | --- | ---------- | ------- | -------- |
|                                           |            |              |          |            |            |               |         | [43] Haotian | Liu,                                          | Chunyuan |     | Li, Yuheng | Li, and | Yong Jae |
| [29] AndrewJaegle,                        |            | FelixGimeno, |          | AndyBrock, |            | OriolVinyals, |         |              |                                               |          |     |            |         |          |
|                                           |            |              |          |            |            |               |         | Lee.         | Improvedbaselineswithvisualinstructiontuning. |          |     |            |         | In       |
| Andrew                                    | Zisserman, |              | and Joao | Carreira.  | Perceiver: |               | General |              |                                               |          |     |            |         |          |
NeurIPS2023WorkshoponInstructionTuningandInstruc-
| perceptionwithiterativeattention. |     |     |     |     | InInternationalconfer- |     |     |                     |     |     |     |     |     |     |
| --------------------------------- | --- | --- | --- | --- | ---------------------- | --- | --- | ------------------- | --- | --- | --- | --- | --- | --- |
|                                   |     |     |     |     |                        |     |     | tionFollowing,2023. |     |     | 1   |     |     |     |
enceonmachinelearning,pages4651–4664.PMLR,2021.
|     |     |     |     |     |     |     |     | [44] Haotian | Liu, | Chunyuan | Li, | Yuheng | Li, Bo Li, | Yuanhan |
| --- | --- | --- | --- | --- | --- | --- | --- | ------------ | ---- | -------- | --- | ------ | ---------- | ------- |
1,5,6
|             |         |                  |         |               |       |          |       | Zhang, | Sheng      | Shen, | and      | Yong Jae | Lee. Llava-next: | Im-   |
| ----------- | ------- | ---------------- | ------- | ------------- | ----- | -------- | ----- | ------ | ---------- | ----- | -------- | -------- | ---------------- | ----- |
| [30] Albert | Q       | Jiang, Alexandre |         | Sablayrolles, |       | Antoine  | Roux, |        |            |       |          |          |                  |       |
|             |         |                  |         |               |       |          |       | proved | reasoning, |       | ocr, and | world    | knowledge, 2024. | 5, 8, |
| Arthur      | Mensch, |                  | Blanche | Savary,       | Chris | Bamford, | De-   |        |            |       |          |          |                  |       |
13
| vendra                 | Singh   | Chaplot,  | Diego | de     | las Casas, | Emma | Bou      |                                                      |             |         |     |          |                       |     |
| ---------------------- | ------- | --------- | ----- | ------ | ---------- | ---- | -------- | ---------------------------------------------------- | ----------- | ------- | --- | -------- | --------------------- | --- |
|                        |         |           |       |        |            |      |          | [45] HaotianLiu,ChunyuanLi,QingyangWu,andYongJaeLee. |             |         |     |          |                       |     |
| Hanna,                 | Florian | Bressand, |       | et al. | Mixtral    | of   | experts. |                                                      |             |         |     |          |                       |     |
|                        |         |           |       |        |            |      |          | Visual                                               | instruction | tuning. |     | Advances | in neural information |     |
| arXiv:2401.04088,2024. |         |           | 1     |        |            |      |          |                                                      |             |         |     |          |                       |     |
|                        |         |           |       |        |            |      |          | processingsystems,36,2024.                           |             |         |     | 1        |                       |     |
[31] PengJin,RyuichiTakanobu,CaiwanZhang,XiaochunCao,
|            |     |             |     |                                |     |     |     | [46] Hao | Liu, Wilson | Yan,              | Matei | Zaharia, | and Pieter   | Abbeel. |
| ---------- | --- | ----------- | --- | ------------------------------ | --- | --- | --- | -------- | ----------- | ----------------- | ----- | -------- | ------------ | ------- |
| andLiYuan. |     | Chat-univi: |     | Unifiedvisualrepresentationem- |     |     |     |          |             |                   |       |          |              |         |
|            |     |             |     |                                |     |     |     | World    | model       | on million-length |       | video    | and language | with    |
powerslargelanguagemodelswithimageandvideounder-
standing. ArXivabs/2311.08046,2024. 2 ringattention. arXivpreprintarXiv:2402.08268,2024. 1
|                        |      |         |       |              |     |           |           | [47] JiajunLiu,YibingWang,HanghangMa,XiaopingWu,Xi- |              |     |      |               |           |         |
| ---------------------- | ---- | ------- | ----- | ------------ | --- | --------- | --------- | --------------------------------------------------- | ------------ | --- | ---- | ------------- | --------- | ------- |
| [32] Rudolph           | Emil | Kalman. | A     | new approach |     | to linear | filtering |                                                     |              |     |      |               |           |         |
|                        |      |         |       |              |     |           |           | aoqi                                                | Ma, Xiaoming |     | Wei, | Jianbin Jiao, | Enhua Wu, | and Jie |
| andpredictionproblems. |      |         | 1960. | 2            |     |           |           |                                                     |              |     |      |               |           |         |
[33] Byung-Kwan Lee, Chae Won Kim, Beomchan Park, and Hu. Kangaroo: Apowerfulvideo-languagemodelsupport-
Yong Man Ro. Meteor: Mamba-based traversal of ratio- inglong-contextvideoinput,2024. 1,8
|      |           |          |     |        |         |       |          | [48] Ilya | Loshchilov, | Frank | Hutter, | et al. | Fixing weight | decay |
| ---- | --------- | -------- | --- | ------ | ------- | ----- | -------- | --------- | ----------- | ----- | ------- | ------ | ------------- | ----- |
| nale | for large | language | and | vision | models. | arXiv | preprint |           |             |       |         |        |               |       |
arXivpreprintarXiv:1711.05101,5,
| arXiv:2405.15574,2024. |             |        | 2    |      |        |        |      | regularizationinadam. |     |     |     |     |     |     |
| ---------------------- | ----------- | ------ | ---- | ---- | ------ | ------ | ---- | --------------------- | --- | --- | --- | --- | --- | --- |
| [34] Bo                | Li, Yuanhan | Zhang, | Dong | Guo, | Renrui | Zhang, | Feng | 2017.                 | 13  |     |     |     |     |     |
Li,HaoZhang,KaichenZhang,YanweiLi,ZiweiLiu,and [49] Hui Lu, Albert Ali Salah, and Ronald Poppe. Videomam-
|          |     |                      |     |      |        |      |           | bapro: | A leap | forward | for | mamba | in video understanding. |     |
| -------- | --- | -------------------- | --- | ---- | ------ | ---- | --------- | ------ | ------ | ------- | --- | ----- | ----------------------- | --- |
| Chunyuan |     | Li. Llava-onevision: |     | Easy | visual | task | transfer. |        |        |         |     |       |                         |     |
arXivpreprintarXiv:2408.03326,2024. 1,2,8 arXivpreprintarXiv:2406.19006,2024. 2
[35] FengLi,RenruiZhang,HaoZhang,YuanhanZhang,BoLi, [50] MuhammadMaaz,HanoonaRasheed,SalmanKhan,andFa-
Wei Li, Zejun Ma, and Chunyuan Li. Llava-next: Tack- hadShahbazKhan. Video-chatgpt: Towardsdetailedvideo
lingmulti-image,video,and3dinlargemultimodalmodels, understandingvialargevisionandlanguagemodels. arXiv
| 2024. | 1   |     |     |     |     |     |     | preprintarXiv:2306.05424,2023. |     |     |     | 1   |     |     |
| ----- | --- | --- | --- | --- | --- | --- | --- | ------------------------------ | --- | --- | --- | --- | --- | --- |
10

[51] KarttikeyaMangalam, RaiymbekAkshulakov, andJitendra Video-xl: Extra-long vision language model for hour-scale
Malik. Egoschema: Adiagnosticbenchmarkforverylong- video understanding. arXiv preprint arXiv:2409.14485,
| formvideolanguageunderstanding. |     |     |     | AdvancesinNeuralIn- |     |     | 2024. | 8   |     |     |     |     |     |     |
| ------------------------------- | --- | --- | --- | ------------------- | --- | --- | ----- | --- | --- | --- | --- | --- | --- | --- |
formationProcessingSystems,36:46212–46244,2023. 2,5, [63] Enxin Song, Wenhao Chai, Guanhong Wang, Yucheng
| 8   |     |     |     |     |     |     | Zhang, | HaoyangZhou, |     | FeiyangWu, |     | XunGuo, | TianboYe, |     |
| --- | --- | --- | --- | --- | --- | --- | ------ | ------------ | --- | ---------- | --- | ------- | --------- | --- |
[52] Meta. Llama3.2: Revolutionizingedgeaiandvisionwith YangLu,Jenq-NengHwang,andGaoangWang.Moviechat:
open,customizablemodels. https://ai.meta.com/blog/llama- From dense token to sparse memory for long video under-
|     |     |     |     |     |     |     | standing. | ArXivabs/2307.16449,2023. |     |     |     | 2   |     |     |
| --- | --- | --- | --- | --- | --- | --- | --------- | ------------------------- | --- | --- | --- | --- | --- | --- |
3-2-connect-2024-vision-edge-mobile-devices,2024.1,3,5,
8,13 [64] Hugo Touvron, Louis Martin, Kevin Stone, Peter Albert,
[53] OpenAI. Introducing chatgpt. Amjad Almahairi, Yasmine Babaei, Nikolay Bashlykov,
https://openai.com/blog/chatgpt,2022. 1 Soumya Batra, Prajjwal Bhargava, Shruti Bhosale, et al.
|                  |     |                 |          |               |        |          | Llama                  | 2: Open | foundation |     | and | fine-tuned | chat | models. |
| ---------------- | --- | --------------- | -------- | ------------- | ------ | -------- | ---------------------- | ------- | ---------- | --- | --- | ---------- | ---- | ------- |
| [54] LongOuyang, |     | JeffreyWu,      | XuJiang, | DiogoAlmeida, |        | Car-     |                        |         |            |     |     |            |      |         |
|                  |     |                 |          |               |        |          | arXiv:2307.09288,2023. |         |            | 1   |     |            |      |         |
| roll Wainwright, |     | Pamela Mishkin, | Chong    |               | Zhang, | Sandhini |                        |         |            |     |     |            |      |         |
Agarwal,KatarinaSlama,AlexRay,etal.Traininglanguage [65] Jiawei Wang, Liping Yuan, and Yuchen Zhang. Tarsier:
|           |        |              |      |       |           | Ad- | Recipes | for training                        |     | and evaluating |     | large video | description |     |
| --------- | ------ | ------------ | ---- | ----- | --------- | --- | ------- | ----------------------------------- | --- | -------------- | --- | ----------- | ----------- | --- |
| models to | follow | instructions | with | human | feedback. |     |         |                                     |     |                |     |             |             |     |
|           |        |              |      |       |           |     | models. | arXivpreprintarXiv:2407.00634,2024. |     |                |     |             | 1           |     |
vancesinneuralinformationprocessingsystems,35:27730–
|             |     |     |     |     |     |     | [66] PengWang,ShuaiBai,SinanTan,ShijieWang,ZhihaoFan, |     |     |     |     |     |     |     |
| ----------- | --- | --- | --- | --- | --- | --- | ----------------------------------------------------- | --- | --- | --- | --- | --- | --- | --- |
| 27744,2022. | 1   |     |     |     |     |     |                                                       |     |     |     |     |     |     |     |
[55] VioricaPatraucean,LucasSmaira,AnkushGupta,AdriaRe- Jinze Bai, Keqin Chen, Xuejing Liu, Jialin Wang, Wenbin
|                         |                                        |               |             |                |            |          | Ge, et                                               | al. Qwen2-vl: |           | Enhancing |                 | vision-language |       | model’s  |
| ----------------------- | -------------------------------------- | ------------- | ----------- | -------------- | ---------- | -------- | ---------------------------------------------------- | ------------- | --------- | --------- | --------------- | --------------- | ----- | -------- |
| casens, LarisaMarkeeva, |                                        | DylanBanarse, |             | SkandaKoppula, |            |          |                                                      |               |           |           |                 |                 |       |          |
|                         |                                        |               |             |                |            |          | perception                                           | of            | the world | at        | any resolution. |                 | arXiv | preprint |
| Mateusz                 | Malinowski,                            | Yi Yang,      | Carl        | Doersch,       | et         | al. Per- |                                                      |               |           |           |                 |                 |       |          |
|                         |                                        |               |             |                |            |          | arXiv:2409.12191,2024.                               |               |           | 8         |                 |                 |       |          |
| ceptiontest:            | Adiagnosticbenchmarkformultimodalvideo |               |             |                |            |          |                                                      |               |           |           |                 |                 |       |          |
|                         |                                        |               |             |                |            |          | [67] XiaohanWang,YuhuiZhang,OrrZohar,andSerenaYeung- |               |           |           |                 |                 |       |          |
| models.                 | Advances                               | in Neural     | Information |                | Processing | Sys-     |                                                      |               |           |           |                 |                 |       |          |
tems,36,2024. 2,5 Levy. Videoagent: Long-form video understanding with
|     |     |     |     |     |     |     | largelanguagemodelasagent. |     |     |     | InEuropeanConferenceon |     |     |     |
| --- | --- | --- | --- | --- | --- | --- | -------------------------- | --- | --- | --- | ---------------------- | --- | --- | --- |
[56] XiaohuanPei,TaoHuang,andChangXu.Efficientvmamba:
|                                               |     |     |     |     |     |       | ComputerVision,pages58–76.Springer,2024. |          |       |      |         |             | 8     |         |
| --------------------------------------------- | --- | --- | --- | --- | --- | ----- | ---------------------------------------- | -------- | ----- | ---- | ------- | ----------- | ----- | ------- |
| Atrousselectivescanforlightweightvisualmamba. |     |     |     |     |     | arXiv |                                          |          |       |      |         |             |       |         |
|                                               |     |     |     |     |     |       | [68] Yi Wang,                            | Kunchang |       | Li,  | Xinhao  | Li, Jiashuo | Yu,   | Yinan   |
| preprintarXiv:2403.09977,2024.                |     |     | 2   |     |     |       |                                          |          |       |      |         |             |       |         |
|                                               |     |     |     |     |     |       | He, Guo                                  | Chen,    | Baoqi | Pei, | Rongkun | Zheng,      | Jilan | Xu, Zun |
[57] VioricaPa˘tra˘ucean,LucasSmaira,AnkushGupta,Adria`Re-
|     |     |     |     |     |     |     | Wang, | et al. | Internvideo2: |     | Scaling | video foundation |     | mod- |
| --- | --- | --- | --- | --- | --- | --- | ----- | ------ | ------------- | --- | ------- | ---------------- | --- | ---- |
casensContinente,LarisaMarkeeva,DylanBanarse,Skanda
|               |            |               |           |             |         |          | els for                | multimodal |         | video | understanding. |                | arXiv | preprint |
| ------------- | ---------- | ------------- | --------- | ----------- | ------- | -------- | ---------------------- | ---------- | ------- | ----- | -------------- | -------------- | ----- | -------- |
| Koppula,      | Joseph     | Heyward,      | Mateusz   | Malinowski, |         | Yi Yang, |                        |            |         |       |                |                |       |          |
|               |            |               |           |             |         |          | arXiv:2403.15377,2024. |            |         | 1     |                |                |       |          |
| Carl Doersch, | Tatiana    | Matejovicova, |           | Yury        | Sulsky, | Antoine  |                        |            |         |       |                |                |       |          |
|               |            |               |           |             |         |          | [69] Ziyang            | Wang,      | Shoubin | Yu,   | Elias          | Stengel-Eskin, |       | Jaehong  |
| Miech, Alex   | Frechette, | Hanna         | Klimczak, |             | Raphael | Koster,  |                        |            |         |       |                |                |       |          |
|               |            |               |           |             |         |          | Yoon,                  | Feng       | Cheng,  | Gedas | Bertasius,     | and            | Mohit | Bansal.  |
JunlinZhang,StephanieWinkler,YusufAytar,SimonOsin-
|                  |       |                                       |           |                   |                |     | Videotree:                                         | Adaptivetree-basedvideorepresentationforllm |            |                                |           |     |          |         |
| ---------------- | ----- | ------------------------------------- | --------- | ----------------- | -------------- | --- | -------------------------------------------------- | ------------------------------------------- | ---------- | ------------------------------ | --------- | --- | -------- | ------- |
| dero, DimaDamen, |       | AndrewZisserman,                      |           | andJoa˜oCarreira. |                |     |                                                    |                                             |            |                                |           |     |          |         |
|                  |       |                                       |           |                   |                |     | reasoningonlongvideos.                             |                                             |            | arXivpreprintarXiv:2405.19209, |           |     |          |         |
| Perception       | test: | A diagnostic                          | benchmark |                   | for multimodal |     |                                                    |                                             |            |                                |           |     |          |         |
|                  |       |                                       |           |                   |                |     | 2024.                                              | 8                                           |            |                                |           |     |          |         |
| videomodels.     |       | InAdvancesinNeuralInformationProcess- |           |                   |                |     |                                                    |                                             |            |                                |           |     |          |         |
|                  |       |                                       |           |                   |                |     | [70] BoWu,ShoubinYu,ZhenfangChen,JoshuaBTenenbaum, |                                             |            |                                |           |     |          |         |
| ingSystems,2023. |       | 5                                     |           |                   |                |     |                                                    |                                             |            |                                |           |     |          |         |
|                  |       |                                       |           |                   |                |     | and Chuang                                         |                                             | Gan. Star: | A                              | benchmark | for | situated | reason- |
[58] YanyuanQiao,ZhengYu,LongtengGuo,SihanChen,Zijia
|                                   |       |        |                |     |              |       | inginreal-worldvideos. |     |        | arXivpreprintarXiv:2405.09711, |     |           |        |     |
| --------------------------------- | ----- | ------ | -------------- | --- | ------------ | ----- | ---------------------- | --- | ------ | ------------------------------ | --- | --------- | ------ | --- |
| Zhao,MingzhenSun,QiWu,andJingLiu. |       |        |                |     | Vl-mamba:Ex- |       |                        |     |        |                                |     |           |        |     |
|                                   |       |        |                |     |              |       | 2024.                  | 5   |        |                                |     |           |        |     |
| ploring state                     | space | models | for multimodal |     | learning.    | arXiv |                        |     |        |                                |     |           |        |     |
|                                   |       |        |                |     |              |       | [71] Haoning           | Wu, | Dongxu | Li,                            | Bei | Chen, and | Junnan | Li. |
| preprintarXiv:2403.13600,2024.    |       |        | 2              |     |              |       |                        |     |        |                                |     |           |        |     |
Longvideobench:Abenchmarkforlong-contextinterleaved
[59] Alec Radford, Jong Wook Kim, Chris Hallacy, Aditya video-languageunderstanding.AdvancesinNeuralInforma-
| Ramesh, | Gabriel | Goh, Sandhini | Agarwal, |     | Girish | Sastry, |                                            |     |     |     |     |     |       |     |
| ------- | ------- | ------------- | -------- | --- | ------ | ------- | ------------------------------------------ | --- | --- | --- | --- | --- | ----- | --- |
|         |         |               |          |     |        |         | tionProcessingSystems,37:28828–28857,2024. |     |     |     |     |     | 2,5,8 |     |
AmandaAskell,PamelaMishkin,JackClark,etal.Learning
|              |        |        |              |          |     |          | [72] JunbinXiao,XindiShang,AngelaYao,andTat-SengChua. |      |       |                       |     |     |               |     |
| ------------ | ------ | ------ | ------------ | -------- | --- | -------- | ----------------------------------------------------- | ---- | ----- | --------------------- | --- | --- | ------------- | --- |
| transferable | visual | models | from natural | language |     | supervi- |                                                       |      |       |                       |     |     |               |     |
|              |        |        |              |          |     |          | Next-qa:                                              | Next | phase | of question-answering |     |     | to explaining |     |
sion.InInternationalconferenceonmachinelearning,pages temporal actions. In Proceedings of the IEEE/CVF con-
8748–8763.PMLR,2021. 5,13 ference on computer vision and pattern recognition,
pages
[60] RuchitRawal,KhalidSaifullah,MiquelFarre´,RonenBasri,
|     |     |     |     |     |     |     | 9777–9786,2021. |     | 2   |     |     |     |     |     |
| --- | --- | --- | --- | --- | --- | --- | --------------- | --- | --- | --- | --- | --- | --- | --- |
David Jacobs, Gowthami Somepalli, and Tom Goldstein. [73] JunbinXiao,XindiShang,AngelaYao,andTat-SengChua.
Cinepile: A long video question answering dataset and Next-qa: Next phase of question-answering to explaining
benchmark. arXivpreprintarXiv:2405.08813,2024. 1 temporal actions. In Proceedings of the IEEE/CVF con-
[61] XiaoqianShen,YunyangXiong,ChangshengZhao,Lemeng
|     |     |     |     |     |     |     | ference | on computer |     | vision | and pattern | recognition, |     | pages |
| --- | --- | --- | --- | --- | --- | --- | ------- | ----------- | --- | ------ | ----------- | ------------ | --- | ----- |
Wu,JunChen,ChenchenZhu,ZechunLiu,FanyiXiao,Bal- 9777–9786,2021. 5,8,13
akrishnanVaradarajan,FlorianBordes,etal. Longvu: Spa- [74] Hu Xu, Saining Xie, Xiaoqing Ellen Tan, Po-Yao Huang,
tiotemporal adaptive compression for long video-language RussellHowes,VasuSharma,Shang-WenLi,GargiGhosh,
understanding. arXivpreprintarXiv:2410.17434,2024. 8 Luke Zettlemoyer, and Christoph Feichtenhofer. Demysti-
[62] Yan Shu, Zheng Liu, Peitian Zhang, Minghao Qin, Jun- fyingclipdata. arXivpreprintarXiv:2309.16671,2023. 5,
| jie Zhou, | Zhengyang | Liang, | Tiejun | Huang, | and | Bo Zhao. | 13  |     |     |     |     |     |     |     |
| --------- | --------- | ------ | ------ | ------ | --- | -------- | --- | --- | --- | --- | --- | --- | --- | --- |
11

[75] Lin Xu, Yilin Zhao, Daquan Zhou, Zhijie Lin, See Kiong understandingwithadvancedlargelanguagemodels. InThe
Ng,andJiashiFeng. Pllava: Parameter-freellavaextension Twelfth International Conference on Learning Representa-
| fromimagestovideosforvideodensecaptioning,2024. |     |     |     |     |     |     | 1, tions,2023.                                       | 1   |
| ----------------------------------------------- | --- | --- | --- | --- | --- | --- | ---------------------------------------------------- | --- |
| 8,13                                            |     |     |     |     |     |     | [89] LianghuiZhu,BenchengLiao,QianZhang,XinlongWang, |     |
[76] Yijun Yang, Zhaohu Xing, and Lei Zhu. Vivim: a video WenyuLiu,andXinggangWang. Visionmamba: Efficient
visionmambaformedicalvideoobjectsegmentation. arXiv visualrepresentationlearningwithbidirectionalstatespace
preprintarXiv:2401.14168,2024. 2 model. arXivpreprintarXiv:2401.09417,2024. 2
[77] QinghaoYe,HaiyangXu,GuohaiXu,JiaboYe,MingYan,
| Yi                                       | Zhou, Junyan | Wang, | Anwen    |     | Hu, Pengcheng |       | Shi, Yaya |     |
| ---------------------------------------- | ------------ | ----- | -------- | --- | ------------- | ----- | --------- | --- |
| Shi,                                     | Chenliang    | Li,   | Yuanhong | Xu, | Hehong        | Chen, | Junfeng   |     |
| Tian,QiangQi,JiChaoZhang,andFeiyanHuang. |              |       |          |     |               |       | mplug-    |     |
owl: Modularizationempowerslargelanguagemodelswith
| multimodality. |     | arXivpreprintarXiv:2304.14178,2023. |     |     |     |     | 1   |     |
| -------------- | --- | ----------------------------------- | --- | --- | --- | --- | --- | --- |
[78] KexinYi,ChuangGan,YunzhuLi,PushmeetKohli,Jiajun
| Wu,           | AntonioTorralba, |            | andJoshuaBTenenbaum. |                |          |                  | Clevrer: |     |
| ------------- | ---------------- | ---------- | -------------------- | -------------- | -------- | ---------------- | -------- | --- |
| Collision     | events           | for        | video                | representation |          | and reasoning.   |          |     |
| In            | International    | Conference |                      | on             | Learning | Representations, |          |     |
| 2020.         | 5                |            |                      |                |          |                  |          |     |
| [79] CeZhang, |                  | TaixiLu,   | MdMohaiminulIslam,   |                |          | ZiyangWang,      |          |     |
| Shoubin       | Yu,              | Mohit      | Bansal,              | and            | Gedas    | Bertasius.       | A sim-   |     |
plellmframeworkforlong-rangevideoquestion-answering.
| arXivpreprintarXiv:2312.17235,2023. |        |     |         |        |       | 8            |     |     |
| ----------------------------------- | ------ | --- | ------- | ------ | ----- | ------------ | --- | --- |
| [80] Hang                           | Zhang, | Xin | Li, and | Lidong | Bing. | Video-llama: | An  |     |
instruction-tunedaudio-visuallanguagemodelforvideoun-
| derstanding.      |        | arXivpreprintarXiv:2306.02858,2023. |                                     |           |            |            | 1     |     |
| ----------------- | ------ | ----------------------------------- | ----------------------------------- | --------- | ---------- | ---------- | ----- | --- |
| [81] Peiyuan      | Zhang, | Kaichen                             |                                     | Zhang,    | Bo Li,     | Guangtao   | Zeng, |     |
| JingkangYang,     |        | YuanhanZhang,                       |                                     |           | ZiyueWang, | HaoranTan, |       |     |
| Chunyuan          |        | Li, and                             | Ziwei                               | Liu. Long | context    | transfer   | from  |     |
| languagetovision. |        |                                     | arXivpreprintarXiv:2406.16852,2024. |           |            |            |       |     |
8
[82] YuanhanZhang,BoLi,haotianLiu,YongjaeLee,Liangke
Gui,DiFu,JiashiFeng,ZiweiLiu,andChunyuanLi.Llava-
| next: | Astrongzero-shotvideounderstandingmodel,2024. |     |     |     |     |     |     |     |
| ----- | --------------------------------------------- | --- | --- | --- | --- | --- | --- | --- |
2,8
[83] YuanhanZhang,JinmingWu,WeiLi,BoLi,ZejunMa,Zi-
| wei                                 | Liu, and | Chunyuan                            | Li.      | Video  | instruction | tuning       | with       |     |
| ----------------------------------- | -------- | ----------------------------------- | -------- | ------ | ----------- | ------------ | ---------- | --- |
| syntheticdata.                      |          | arXivpreprintarXiv:2410.02713,2024. |          |        |             |              | 8          |     |
| [84] Han                            | Zhao,    | Min Zhang,                          | Wei      | Zhao,  | Pengxiang   | Ding,        | Siteng     |     |
| Huang,                              | and      | Donglin                             | Wang.    | Cobra: | Extending   | mamba        | to         |     |
| multi-modal                         |          | large                               | language | model  | for         | efficient    | inference. |     |
| arXivpreprintarXiv:2403.14520,2024. |          |                                     |          |        |             | 2            |            |     |
| [85] ZijiaZhao,                     |          | HaoyuLu,                            | YuqiHuo, |        | YifanDu,    | TongtianYue, |            |     |
| Longteng                            | Guo,     | Bingning                            |          | Wang,  | Weipeng     | Chen,        | and Jing   |     |
Liu.Needleinavideohaystack:Ascalablesyntheticframe-
| workforbenchmarkingvideomllms. |     |     |       |     | arXive-prints,pages |     |     |     |
| ------------------------------ | --- | --- | ----- | --- | ------------------- | --- | --- | --- |
| arXiv–2406,2024.               |     |     | 2,5,8 |     |                     |     |     |     |
[86] JunjieZhou,YanShu,BoZhao,BoyaWu,ShitaoXiao,Xi
Yang,YongpingXiong,BoZhang,TiejunHuang,andZheng
| Liu. | Mlvu: | A comprehensive |     |     | benchmark | for multi-task |     |     |
| ---- | ----- | --------------- | --- | --- | --------- | -------------- | --- | --- |
longvideounderstanding.arXivpreprintarXiv:2406.04264,
| 2024.              | 2,5,8                                        |           |               |           |       |                   |         |     |
| ------------------ | -------------------------------------------- | --------- | ------------- | --------- | ----- | ----------------- | ------- | --- |
| [87] Luowei        | Zhou,                                        | Chenliang |               | Xu, and   | Jason | Corso.            | Towards |     |
| automatic          |                                              | learning  | of procedures |           | from  | web instructional |         |     |
| videos.            | InProceedingsoftheAAAIConferenceonArtificial |           |               |           |       |                   |         |     |
| Intelligence,2018. |                                              |           | 5             |           |       |                   |         |     |
| [88] Deyao         | Zhu,                                         | Jun Chen, | Xiaoqian      |           | Shen, | Xiang Li,         | and Mo- |     |
| hamed              | Elhoseiny.                                   |           | Minigpt-4:    | Enhancing |       | vision-language   |         |     |
12

BIMBA: Selective-Scan Compression for Long-Range Video Question Answering
|     |     |     |     |     | Supplementary |     |     | Material |     |     |     |     |
| --- | --- | --- | --- | --- | ------------- | --- | --- | -------- | --- | --- | --- | --- |
Our supplementary materials contain additional imple- Here, too, we observe that the relative performance gap
mentation details (Section S1), additional quantitative re- widensasvideodurationincreases,showcasingtheadvan-
sults(SectionS2),andqualitativeresults(SectionS3). tages of our model over the vanilla LLaMA-3.2 (video)
baseline,whichdoesnotuseanycompressionmechanism.
S1.AdditionalImplementationDetails
S2.2.ComputationCostofBIMBA-LLaMA
| BIMBA-LLaVA |     | is based | on the | image-pretrained |     |     | MLLM |                  |            |                   |      |        |
| ----------- | --- | -------- | ------ | ---------------- | --- | --- | ---- | ---------------- | ---------- | ----------------- | ---- | ------ |
|             |     |          |        |                  |     |     |      | In this section, | we compare | the computational | cost | of our |
LLaVA-NeXT[44],whichutilizesCLIP[59]asthevision modelwithotherbaselinesintermsofGPUmemoryusage
| encoder      | and Vicuna-7B |              | [11] as | the LLM. | It   | processes | 64   |                                            |     |     |     |           |
| ------------ | ------------- | ------------ | ------- | -------- | ---- | --------- | ---- | ------------------------------------------ | --- | --- | --- | --------- |
|              |               |              |         |          |      |           |      | (FigureS2,left)andruntime(FigureS2,right). |     |     |     | Ouranaly- |
| video frames | at            | a resolution | of      | 336 ×    | 336, | dividing  | each |                                            |     |     |     |           |
sisshowsthatself-attentionincursquadraticcostsforboth
frameinto14×14patches,yielding64×24×24spatiotem-
memoryandruntime,resultinginout-of-memoryerrorsfor
| poraltokens. | Thesetokensarecompressedto16×12×12 |     |     |     |     |     |     |                                        |     |     |                |     |
| ------------ | ---------------------------------- | --- | --- | --- | --- | --- | --- | -------------------------------------- | --- | --- | -------------- | --- |
|              |                                    |     |     |     |     |     |     | inputslongerthan8frames(12,800tokens). |     |     | Incontrast,all |     |
before being fed into the LLM. In this variant, the vision othermethodsmaintainlowmemoryandruntimecosts.De-
| encoder | remains | frozen, | while | the multimodal |     | projector | (a  |     |     |     |     |     |
| ------- | ------- | ------- | ----- | -------------- | --- | --------- | --- | --- | --- | --- | --- | --- |
spitehavingcomputationalefficiencysimilartothatofthe
| linear layer), | spatiotemporal |     | token | selector, |     | and LLM | are |     |     |     |     |     |
| -------------- | -------------- | --- | ----- | --------- | --- | ------- | --- | --- | --- | --- | --- | --- |
otherbaselines,ourmethodachievessuperiorperformance,
trainedusingLoRA[23].
asdemonstratedintheprevioussection.
BIMBA-LLaMAisbasedontheimage-pretrainedMLLM
LLaMA-3.2 [52], incorporating Meta-CLIP [74] as the vi- S3.QualitativeResults
sionencoderandLLaMA-3.2-LLM-8BastheLLM.Itpro-
cesses64videoframesatahigherresolutionof560×560, Our qualitative results include open-ended video question
dividing each frame into 14 × 14 patches, resulting in answering (Section S3.1), multiple choice video question
64×40×40spatiotemporaltokens. Thesearecompressed answering (Section S3.2), importance of question condi-
|             |     |        |       |        |        |      |        | tioning (Section | S3.3), | and significance | of bidirectional |     |
| ----------- | --- | ------ | ----- | ------ | ------ | ---- | ------ | ---------------- | ------ | ---------------- | ---------------- | --- |
| to 16×20×20 |     | before | being | passed | to the | LLM. | Unlike |                  |        |                  |                  |     |
theothervariants, boththevisionencoderandmultimodal Mambaandinterleavedqueries(SectionS3.4).
projectorremainfrozen,withonlythespatiotemporaltoken
S3.1.Open-EndedVideoQuestionAnswering
selectorandLLMtrainedusingLoRA.
Training Details. We employ standard cross-entropy loss In Figure S3, we provide examples of our model’s perfor-
forautoregressivetextgenerationandtrainthemodelfor1 mance in open-ended video question answering. The re-
epoch with a batch size of 128 and a learning rate of 2e- sults showcase the model’s ability to handle diverse video
5. TheAdamW[48]optimizerisused,alongwithacosine
understandingtasks,includinggeneratingdetaileddescrip-
learningrateschedulerandawarm-upratioof0.03. tions,recognizingobjectsandinteractions,identifyingfine-
|     |     |     |     |     |     |     |     | grained activities, | and | inferring high-level | goals. | These |
| --- | --- | --- | --- | --- | --- | --- | --- | ------------------- | --- | -------------------- | ------ | ----- |
S2.AdditionalQuantitativeResults examples illustrate the model’s effectiveness in general-
purposevideounderstanding.
S2.1.PerformanceasaFunctionofVideoLength
S3.2.MultipleChoiceVideoQuestionAnswering
| In this section, | we  | evaluate | the | performance |     | of our | model |     |     |     |     |     |
| ---------------- | --- | -------- | --- | ----------- | --- | ------ | ----- | --- | --- | --- | --- | --- |
onvideosofvaryinglengthsfromtheNextQA[73]dataset, Weshowqualitativeexamplesofvideoquestionanswering
with results presented in Figure S1. Figure S1 (left) of our model and other baselines on NextQA (Figure S4)
andEgoSchema(FigureS5)datasets.BothBIMBA-LLaVA
| shows the | relative | performance |     | improvement |     | over | the |     |     |     |     |     |
| --------- | -------- | ----------- | --- | ----------- | --- | ---- | --- | --- | --- | --- | --- | --- |
PLLaVA [75] baseline for different video durations. We and BIMBA-LLaMA generate the correct answers while
observe that as video duration increases, the relative per- other baselines fail, demonstrating the effectiveness of our
| formance    | improvement |              | over | the baseline      | becomes |     | more   | modelforthistask. |     |     |     |     |
| ----------- | ----------- | ------------ | ---- | ----------------- | ------- | --- | ------ | ----------------- | --- | --- | --- | --- |
| pronounced. | This        | demonstrates |      | the effectiveness |         |     | of our |                   |     |     |     |     |
S3.3.ImportanceofQuestionConditioning
proposedMamba-basedtokencompressiontechniquecom-
paredtopooling-basedmethods,particularlyforlong-range In Figure S6, we showcase example predictions from our
videos. model with and without question-conditioned token selec-
Similarly, the Figure S1 (right) illustrates the rela- tion on the NextQA (Figure S6 (a)) and EgoSchema (Fig-
tiveperformanceimprovementofBIMBA-LLaVAoverthe ure S6 (b)) datasets. In both cases, incorporating question
LLaMA-3.2 (video) baseline for varying video durations. tokens into our spatiotemporal token selector enables the
13

|                      | 30  |     |     |     |     |                      | 20  |     |     |
| -------------------- | --- | --- | --- | --- | --- | -------------------- | --- | --- | --- |
| tnemevorpmIycaruccA% |     |     |     |     |     | tnemevorpmIycaruccA% |     |     |     |
15
20
10
10
5
|     | 0   |                            |         |     |     |     | 0                          |         |     |
| --- | --- | -------------------------- | ------- | --- | --- | --- | -------------------------- | ------- | --- |
|     | 0   | 50                         | 100     |     | 150 |     | 0 50                       | 100     | 150 |
|     |     |                            | Seconds |     |     |     |                            | Seconds |     |
|     |     | (a)LLaVABackboneonNeXT-QA. |         |     |     |     | (b)LLaMABackboneonNeXT-QA. |         |     |
FigureS1.Relativeperformanceimprovementof(left)BIMBA-LLaVAoverPLLaVAbaselineand(right)BIMBA-LLaMAoverLLaMA-
3.2(video)baselinefordifferentvideodurationsonNextQAdataset.Ourmodelachieveslargergainsasthevideolengthincreases.
3
60
| )BG(yromeM |     |     |     |     |     | )ceS(emitnuR |     |     |     |
| ---------- | --- | --- | --- | --- | --- | ------------ | --- | --- | --- |
2
40
|     |     |     |     | BIMBA          |     |     |     | BIMBA          |     |
| --- | --- | --- | --- | -------------- | --- | --- | --- | -------------- | --- |
|     |     |     |     | Self-Attention |     |     |     | Self-Attention |     |
1
|     |     |     |     | Perceiver |     |     |     | Perceiver |     |
| --- | --- | --- | --- | --------- | --- | --- | --- | --------- | --- |
|     | 20  |     |     | Pooling   |     |     |     | Pooling   |     |
|     |     |     |     | Vanilla   |     |     |     | Vanilla   |     |
0
|     |     | 20K 40K | 60K    | 80K | 100K |     | 20K 40K | 60K 80K | 100K |
| --- | --- | ------- | ------ | --- | ---- | --- | ------- | ------- | ---- |
|     |     |         | Tokens |     |      |     |         | Tokens  |      |
(a)MemoryUsageofLLaMA-3.2Backbone. (b)RuntimeofLLaMA-3.2Backbone.
FigureS2. ComputationcostofBIMBA-LLaMAandbaselinemodelsintermsofmemoryusage(left)andruntime(right). Self-attention
runsoutofmemoryforlongersequences.Allotherbaselines,includingourmodel,maintainlowmemoryandruntime.
modeltoproducethecorrectanswer. Thisexhibitstheabil- and end of the video, and (c) with unidirectional Mamba
ity of our token selector to leverage question tokens effec- and standard queries, the model focuses only on the latter
tively,selectingrelevantvisualtokenstoenhancequestion- partofthevideo. Bothdesignsaresuboptimal,astheymiss
answeringperformance. criticalinformationandproduceincorrectanswers.
S3.4.BidirectionalMambaandInterleavedQueries
| In                                  | this section, | we visualize  |             | the effect | of              | bidirectional |     |     |     |
| ----------------------------------- | ------------- | ------------- | ----------- | ---------- | --------------- | ------------- | --- | --- | --- |
| Mamba                               | and           | interleaved   | queries     | in Figure  |                 | S7. We calcu- |     |     |     |
| latearesponseforeachframeasfollows: |               |               |             |            | first,wetakethe |               |     |     |     |
| hidden                              | states        | of each token | after       | the        | spatiotemporal  | token         |     |     |     |
| selector                            | and           | compute a     | dot product | with       | the             | query tokens. |     |     |     |
Then,weapplymaxpoolingtothedotproductvaluesofto-
kenswithineachframetoobtainaresponseforthatframe.
Thisresponsevaluereflectstheweightofeachframeinthe
compressedqueryrepresentations.
|     | Figure | S7 (a) shows | that | using bidirectional |     | scans and |     |     |     |
| --- | ------ | ------------ | ---- | ------------------- | --- | --------- | --- | --- | --- |
interleavedqueriesenablesourmodeltocapturecriticalin-
| formation |     | across the entire | video | and | generate | the correct |     |     |     |
| --------- | --- | ----------------- | ----- | --- | -------- | ----------- | --- | --- | --- |
answer. Incontrast,(b)withbidirectionalMambaandstan-
| dard | queries, | the model | focuses | mainly | on  | the beginning |     |     |     |
| ---- | -------- | --------- | ------- | ------ | --- | ------------- | --- | --- | --- |
14

(a)Example1ofopen-endedvideoquestionanswering.
(b)Example2ofopen-endedvideoquestionanswering.
FigureS3. QualitativeResultsonOpen-EndedVideoQuestionAnswering. Ourmodeldemonstratestheabilitytoanswerawiderangeof
questionsaboutvideos,includingdetaileddescriptions,high-levelgoals,andfine-grainedactivities.
15

FigureS4.QualitativeResultsonNextQA.OurmodelgeneratesthecorrectanswerwhilebothPLLaVAandLLaMA-3.2(video)baselines
fail.
16

Figure S5. Qualitative Results on EgoSchema. Our model generates the correct answer while both PLLaVA and LLaMA-3.2 (video)
baselinesfail.
17

Figure S6. Qualitative Results on Question Conditioned Token Selection on (a) NextQA and (b) EgoSchema datasets. Incorporating
question tokens into our spatiotemporal token selector leads to the correct answer in both examples. Using the information from the
questionsallowsourspatiotemporalselectionmoduletofocusonthemostrelevantvideopartsforansweringthequestion.
18

FigureS7. VisualizationofBidirectionalMambaandInterleaveQueries. UtilizingbidirectionalMambaandinterleavedqueriesleadsto
thecorrectanswer,whiletheunidirectionalMambaandstandardqueriesfail.
19