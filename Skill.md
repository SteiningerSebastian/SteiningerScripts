# StudyOER Script Authoring Skill

This skill teaches an AI how to author **StudyOER scripts** — the Markdown learning
documents in this repository — so that new scripts match the existing ones in
structure, tone, and (critically) render correctly on the StudyOER website
(<https://studyoer.logicbystein.com/>).

When asked to write or extend a script, follow this skill to the letter.

---

## 1. What a script actually is

A script is a plain **Markdown** file (`.md`) that StudyOER fetches over HTTP and
renders with a **custom renderer**, not with a generic Markdown engine. Because the
renderer is custom, *what you write in Markdown is only half the story* — the renderer
maps specific Markdown constructs to specific Vue components. Writing a "valid"
Markdown file is not enough; it must use the constructs the renderer understands.

Key implication: **you do not control the HTML output.** You control the Markdown
*input*, and the renderer decides how it looks. So the single most important rule is
to use the exact constructs listed in section 3.

---

## 2. The rendering pipeline (what happens to your Markdown)

1. The file is parsed with **commonmark** (the `commonmark` JS library).
2. A custom `VuetifyHTMLRenderer` (extends commonmark's `HtmlRenderer`) walks the AST
   and emits HTML, *plus* a list of Vue components (`VNode`s) mounted into placeholder
   `<div>`/`<ul>`/`<ol>` elements.
3. Construct → component mapping:

   | Markdown construct | Rendered as |
   |---|---|
   | `#` / `##` / `###` headings | `<section>` wrapper + auto-numbered heading, added to the Table of Contents |
   | `####`+ headings | plain heading, **not** numbered, **not** in TOC |
   | Fenced code block with language `tex` | `MathView` (MathJax/CHTML rendered formula) |
   | Fenced code block with any other language | `CodeView` (highlight.js with syntax highlighting + copy button) |
   | Image whose URL contains `img.youtube.com/vi/` and ends in `.jpg` | `VideoView` (embedded YouTube player) |
   | Any other image | `ImageView` |
   | Bulleted / numbered list | `ListView` (presentation-aware) |
   | Table | `TableView` (via a **post-processor**, *not* commonmark) |
   | `> blockquote` | regular `<blockquote>` — used for chapter **Summaries** |

4. **Tables are special:** commonmark does *not* render GFM tables, so a post-processor
   (`MarkdownHTMLPostProcessorTable`) scans the raw HTML for pipe `|` sequences and
   builds a `TableView` from them. This means table syntax is fragile — follow the
   exact format in section 3.6 or the table will not render at all.

5. **Headings are section boundaries.** Each `#`/`##`/`###` opens a `<section>` element.
   In presentation mode, sections are the navigation units (up/down arrow keys jump
   between them). Heading numbers (`1.`, `1.1`, `1.1.1`) are generated *automatically*
   by the renderer — **do not type them yourself.**

---

## 3. Exact Markdown constructs to use

### 3.1 Headings and structure (the skeleton)

- Use `#` (H1) for **chapters** only. Use `##` (H2) for major topics within a chapter,
  and `###` (H3) for sub-topics.
- **Only H1–H3 are numbered and appear in the Table of Contents.** H4+ are invisible
  to the TOC and to presentation navigation. Avoid `####` unless you truly need a
  non-chapter heading.
- Do **not** write your own numbering (`1.2.3 Foo`); the renderer prepends
  `1.` / `1.1.` / `1.1.1` automatically based on heading level.
- **NEVER use punctuation that the parser treats as inline markup inside a heading.**
  The renderer derives the heading id and TOC label from `node.firstChild.literal` — the
  text of the **first** inline child only. Any heading containing `**bold**`, `` `code` ``,
  `*italic*`, `[a link](…)`, or even a `'` or `"` character is **silently truncated** in
  the Table of Contents and the `#anchor` (the visible heading body still shows fully).
  See the verified table in section 10.6.
  Write headings as **plain text**: `## Azure - Creating an Account`, not
  `## **Azure** - Creating an Account` and not `## What a "script" is`.- **Do not put dates, session numbers or week numbers in headings.** A script is often
  taught at a different pace than it was written: a cohort may need seven sessions
  instead of six, one lab may run short, another long. Headings must stay **topic
  based** and survive any schedule. Write `## Priorities and Fixed Priority Preemption`,
  never `## Session 3 - Priorities` or `## Week 4`. If a schedule is genuinely useful
  information, put it in a **table in the Introduction prose**, where it can be adjusted
  without breaking the structure of the document and the Table of Contents.- Every script must start with the standard three-part skeleton (see section 4).

### 3.2 Emphasis

- **Bold** (`**text**`) is used *heavily* for key terms, concepts, and lead-ins. It is
  the primary visual tool in these scripts. Use it liberally but for genuine key terms.
- Italics (`*text*` / `_text_`) appear occasionally for inline examples/quoted prompts.
- Do **not** use `~~strikethrough~~`, task lists (`- [ ]`), or HTML tags — the renderer
  does not support them reliably.

### 3.3 Lists

Two kinds, both render via `ListView`:

- Bulleted: `- item`
- Ordered: `1. item`

**The dominant list idiom** in every script is a bold lead-in term followed by a colon
and an explanation:

```markdown
- **On-demand self-service**: Users can provision resources automatically without human intervention.
- **Broad network access**: Services are available over the network and accessible through standard devices.
```

This idiom matters for presentation mode (see section 9) — the bold lead-in becomes the
visible "bullet" and the trailing explanation is hidden until needed.

**The lead-in must name the thing, not the category.** This is the single most common
mistake and it ruins a presentation slide. If the bold part is a label like *Strength*,
*Weakness* or *Typical use*, then a slide showing only bold text says nothing and the
actual content is hidden.

```markdown
❌ BAD  - **Strength:** No abstractions, nothing between your code and the hardware.
❌ BAD  - **Weakness:** Every job shares one path, a blocking call stalls everything.
❌ BAD  - **Typical use:** A light switch, a lamp dimmer, a sensor node.

✅ GOOD - **No abstractions:** Nothing stands between your code and the hardware.
✅ GOOD - **One path only:** Every job shares one path, a blocking call stalls everything.
✅ GOOD - **Simple devices only:** A light switch, a lamp dimmer, a sensor node.
```

Read the GOOD version aloud as a list of bullets and it still makes sense. Read the BAD
version aloud and you have said "Strength, Weakness, Typical use", which is a table of
contents for information the audience cannot see.

- **Rule:** the bold lead-in is the **content**, the tail is the **detail**. Swap them if
  you catch yourself writing a category word before the colon.
- Put blank lines between list items / paragraphs for readability.
- Do not nest lists deeper than one level; the renderer's list handling is simple.

### 3.4 Code blocks

Use fenced code blocks with a **language tag**:

````markdown
```python
print("test")
```
````

- The language tag feeds **highlight.js** (`highlight.js/lib/common`). Stick to common
  language names it recognizes (`python`, `javascript`, `typescript`, `csharp`, `cpp`,
  `java`, `powershell`, `bash`, `sql`, `json`, `yaml`, `dockerfile`, `html`, `css`, …).
  If unknown, highlight.js falls back to autodetect (still works, just no control).
- The code block renders with a **copy button** in normal mode.
- **Never hard-code a device port as if it were universal.** Writes like
  `idf.py -p COM5 flash` teach students that `COM5` is correct, and it is wrong on most
  machines. Use a placeholder and say where to find the real value:

  ```powershell
  idf.py -p <YOUR_PORT> flash monitor
  ```

  Then explain in prose: "`<YOUR_PORT>` is whatever port your board shows up as, for
  example `COM5`, `COM12`, `/dev/ttyUSB0`; find it in Device Manager under Ports, or run
  `idf.py -p detect`."

**Math is a special code block:** use the language tag `tex` to render a formula via
MathJax:

````markdown
```tex
\sqrt{\sum_{i=1}^{n} (p_i - q_i)^2}
```
````

- **Whitespace inside a `tex` block is stripped before MathJax sees it.** This is the
  most common math bug in these scripts. A backslash command followed by a space and then
  a letter fuses into one invalid command: `\cdot d` becomes `\cdotd`, and the rendered
  output is the literal text **`Undefined control sequence \cdotd`** instead of a formula.
  Verified in production (2026-11-01).
- **Fix: separate a command from a following letter with `\,`**, not with a space.

  ```tex
  v = \frac{\pi\, \cdot\, n\, \cdot\, d}{60}
  ```

  `\pi n` (a command directly followed by a letter, no space) is fine — it is only the
  space-then-letter case that breaks.
- **Avoid `\text{...}` and `\newline` inside `tex` blocks.** Both are unreliable in this
  renderer. Keep the block **pure mathematics** and move every unit, word and multi-line
  explanation into the prose immediately before or after the block. This is also better
  pedagogy: a formula plus a sentence naming each symbol beats a formula with words
  crammed inside it.
- Do **not** use inline `$...$` math — the renderer does not support it. Math must be in
  a `tex` fenced block.
- **Verification check:** after rendering, run
  `document.body.innerText.match(/Undefined control sequence/g)` and require `null`.
  Also assert that `document.querySelectorAll('math').length` equals the number of `tex`
  fences in the file. A missing block or a control-sequence error always means this bug.

### 3.5 Images and videos

**Images** — reference local files in the script's `./files/` directory (relative paths
are resolved relative to the Markdown file's location):

```markdown
![Cloud Computing](./files/HistoryOfCloudComputing.svg)
```

- Relative paths (`./...` or `/...`) are rewritten to be relative to the hosting
  repository. Use `./files/...` for assets shipped with the script.
- Absolute `https://...` URLs are passed through unchanged.

**A PDF is not an image.** `![x](./files/something.pdf)` does **not** render — the
`ImageView` component expects an image format. If the source material is a PDF (a
schematic, a datasheet, a diagram), **render every page to a PNG** and place the PNGs:

```powershell
python -c "import pymupdf; d=pymupdf.open('files/Thing.pdf'); [p.get_pixmap(dpi=200).save(f'files/Thing-p{i+1}.png') for i,p in enumerate(d)]"
```

- **One page, one image, one talking point.** Place each page in its own section right
  under a sentence that says what to look at, then explain the page. A single embedded
  PDF link forces the reader to leave the page and hunt.
- **Use 200 dpi** for schematic-style vector PDFs: sharp on a projector, and the file
  stays small enough to load quickly.
- **Crop with the PDF, not with a screenshot** when a detail matters
  (`page.get_pixmap(dpi=380, clip=Rect(...))` gives a clean, readable zoom of a small
  region such as a connector pinout).
- **Still link the original PDF** as a download if students need to zoom further, but
  never rely on the link alone — explain the content in text and images.

**Downloadable files (code, starter kits, archives).** A relative link to a zip under
`files/` **does not download** — the renderer treats it as a page route, not as a file
transfer. Link to the **raw file on GitHub** instead, so the browser downloads it directly
from that server:

```markdown
[Download the HAL starter kit](https://github.com/SteiningerSebastian/SteiningerScripts/raw/refs/heads/main/files/robotit-hal-starter.zip)
```

- **Use the `/raw/refs/heads/main/` form**, not `blob/`. The `blob` URL opens GitHub's
  file viewer; only the `raw` URL serves the bytes. Verify it once with a `HEAD` request:
  the answer must be `200` with `Content-Type: application/zip`.
- **Relative `./files/…` links still work for images and PDFs** (section 3.5) — only the
  actual **download** of a binary needs the absolute raw URL. Do not convert image links.
- **Assets must be committed and pushed** before the link can resolve. A link that works
  locally but points at an unpushed file is a 404 for every student.
- **Never reference code that does not exist.** If the text says "look at the reference
  firmware", there must be a real, downloadable archive behind that sentence, or the code
  must be short enough to appear inline in the script. A dangling reference to a private
  repository is worse than no reference at all: the student cannot follow the lesson.
- Zip contents should unzip into a **usable project**, not a pile of loose files. Include
  a `README.md` at the archive root and verify the entry names after zipping.

**Videos** — embed a YouTube video by using its **thumbnail image** as an image link.
The renderer detects `img.youtube.com/vi/…` ending in `.jpg` and swaps in an embedded
player:

```markdown
[![Parallax](https://img.youtube.com/vi/2z4OTRFuLP8/0.jpg)](https://www.youtube.com/embed/2z4OTRFuLP8?si=2QOXovyinn-UT5gJ)
```

- Use the `/0.jpg` thumbnail for the default frame (or `/2.jpg` for a later frame).
- The `<id>` in the thumbnail *is* the video id — make sure the link target is that same
  video. The embed URL (`youtube.com/embed/...` vs `youtu.be/...`) does not matter for
  detection, but keep the thumbnail correct.

### 3.6 Tables

Tables render through a fragile pipe-scanner. Use the simple, well-formed format:

```markdown
|Header 1|Header 2|Header 3|
|---|---|---|
|value|value|value|
|value|value|value|
```

- Always include the separator row `|---|---|---|` — this tells the renderer the first
  row is a header. Without it the header row is treated as data.
- **No formatting inside cells (no bold, no inline code)** — the parser treats cell text
  literally, so `` `motor_init` `` renders with the backticks visible and `**bold**`
  renders with the asterisks visible. Write `motor_init` as plain text in a cell, and
  explain that these are function names in the prose around the table.
- Keep tables small; the scanner mis-parses complex/escaped `|` usage. Do **not** put
  literal pipes inside cells.

### 3.7 Links

Standard Markdown links work and open in a new tab:

```markdown
[Azure Students DE](https://azure.microsoft.com/de-de/free/students)
```

### 3.8 Chapter Summaries (blockquote)

Every chapter should end with a `>` **Summary** blockquote. Inside summaries, use plain
text plus an optional bulleted recap:

```markdown
> Summary
> The cloud delivers on-demand, self-service IT resources.
> Key characteristics:
> - **On-demand self-service**
> - **Resource pooling**
> - **Measured service**
>
> By Joda
```

---

## 4. The standard script skeleton

Copy this structure for every new script:

```
<optional author/LLM attribution line>
# Learning Objectives
## Knowledge and Understanding
- **<Area>:** <what students will understand — bold lead-in + explanation>
... (5–7 items)

## Skills and Abilities
- **<Capability>:** <what students will be able to do>
... (4–6 items)

# Introduction
<2–4 paragraphs of bold-heavy prose introducing the topic>
<optional intro YouTube video embed>

# <Chapter 1 title>
## <topic>
<prose, lists, images, code>
> Summary...

# <Chapter 2 title>
... etc.
```

Notes:

- `# Learning Objectives` is always the first H1 and uses exactly two H2s:
  **`## Knowledge and Understanding`** and **`## Skills and Abilities`**, each a bulleted
  list of bold-lead-in items.
- `# Introduction` follows, with prose + a welcome video.
- Each subsequent H1 is a chapter, ending in a `>` summary blockquote.
- Some scripts carry an **attribution line** at the very top, e.g.:
  `This script was developed and refined with the assistance of advanced language models, including Copilot, Gemini, and others…`
  Include a short, honest attribution/credit line when appropriate.

### 4.1 Writing a lab script (a session based course)

A laboratory script differs from a lecture script in one important way: **the room sets
the pace, not the document.** Some cohorts need seven sessions, some need eight, one
group finishes the theory early and one group never gets the hardware working in week
one. Author for that reality.

- **Chapters are topics, not sessions.** `# Priorities and Fixed Priority Preemption` is a
  chapter. `# Session 3 - Priorities` is a hostage to the calendar.
- **Give the schedule once, in the Introduction, as a table.** A table of "what happens in
  which block" can be edited in one place; heading numbers scattered through the document
  cannot.
- **Every chapter must stand alone enough to be run first.** Students miss sessions. A
  chapter that only makes sense if the previous one happened is fragile — open it with a
  one line reminder of what it depends on.
- **Never say "as we saw last week".** Say "the previous chapter showed that…", which stays
  true whether the gap was four days or two weeks.
- **Front-load setup that can fail.** If installing a toolchain or a driver can take thirty
  minutes or go wrong, it belongs at the very beginning of the course, not in the middle:
  start it early, let it download while teaching continues, and expect a bug list.
- **Collect known failure modes in one place.** Students hit the same four problems every
  year. Put them in a short troubleshooting table rather than in a paragraph of prose, and
  keep a link to a longer documented list if one exists.
- **Quizzes cover the previous chapter, not the current one.** A quiz at the start of a
  session is retrieval practice for what was already taught; a quiz at the end tests
  whether the student was listening five minutes ago.
- **A project needs a fallback that is not "wait for hardware".** If the lab depends on a
  device that may be unavailable, provide a simulated path so the learning continues
  (section 3.5 covers shipping that fallback).

### 4.2 Hardware safety in lab scripts

Lab scripts are executed by students on real equipment. Never write a task that can
damage hardware or hurt a person, even if it would be a nice measurement.

- **Do not ask students to probe a battery, a charger or a live power rail** with a
  multimeter. Lithium cells, boost converters and chargers do not forgive a slip. If the
  learning objective is "understand the voltage divider", have the firmware print the
  measured value and let the student reason about the ratio from the numbers.
- **Put the safe observation in the code.** A printed reading, a log line or an LED code
  teaches the same concept with none of the risk.
- **State the physical safety rule explicitly** where it applies: lift the robot before
  the first motor run, keep hands clear, use a power switch that is reachable.
- **If a measurement must be made physically, name the safe procedure** (power off, cell
  removed, only the designated test point) instead of leaving the method to the student.
- **Never publish a pin, rail or voltage figure you have not verified** against the
  schematic or the hardware.

---

## 5. Tone and writing style

- **Bold-heavy, concept-driven prose.** Nearly every sentence highlights key terms in
  bold. This is a deliberate, consistent style — match it.
- **Expository / textbook tone**, second-person ("you will", "students will") for
  learning objectives; neutral explanatory tone in the body.
- **Concrete examples and named tools** (Azure, Docker, Kubernetes, MathJax) are
  preferred over abstraction.
- **Keep paragraphs focused** and separated by blank lines.
- **Lists with bold lead-ins** (`**Term:** explanation`) are the default way to present
  definitions, key points, features, pros/cons, and step sequences. Prefer them over long
  walls of prose.

---

## 6. Pedagogy and didactics — plan the learning journey

A StudyOER script is not a reference manual — it is a **lesson**. Every chapter must be
built so a student who has never seen the topic can progress from *no knowledge* to
*confident understanding and skill*. Apply these principles at the chapter level.

### 6.1 Align everything with the learning objectives

- The `# Learning Objectives` block is a **contract**. Every chapter must trace back to at
  least one objective in `## Knowledge and Understanding` (what they *know*) or
  `## Skills and Abilities` (what they *do*).
- Write objectives as **measurable behaviors**, not vague outcomes. Prefer *"will be able
  to design and deploy an Azure Web App"* over *"will learn about Azure"*.
- If a chapter teaches something not in the objectives, either add an objective or cut it.

### 6.2 Activate prior knowledge first

- Begin a topic by linking it to what a student already knows or has just learned in a
  previous chapter ("Recall that virtualization lets multiple systems share hardware…").
- Use a **comparison or analogy** early to anchor the abstract in the concrete (e.g.
  *"a Resource Group is like a folder that holds everything a project needs"*).
- The `# Introduction` chapter should give the *why* before the *how* — motivate the
  learner by naming real problems the topic solves before diving into mechanics.

### 6.3 Manage cognitive load - chunk, do not overwhelm

- **One idea per heading, one point per list item, one thought per paragraph.** The
  headings already form slides in presentation mode — use that to your advantage.
- Break a complex concept into **short, progressive steps** (an ordered list with bold
  lead-ins) rather than a dense paragraph.
- Introduce **one new term / concept at a time**, define it immediately, then build on it.
  Don't stack three new terms in one sentence.
- Prefer the **concrete → abstract** order: show a specific example first, then state the
  general principle it illustrates.

### 6.4 Follow a consistent learning cycle within each topic

Where space allows, work through this sequence (not every topic needs every step, but the
order should hold):

1. **Explain** — introduce the concept in prose with bold key terms.
2. **Illustrate** — add a diagram (`./files/…`), a concrete example, or a short analogy.
3. **Demonstrate** — show a ```` ```code ```` ` block or a video of it in action.
4. **Practice** — give the student something to *do*: a command to run, an exercise, or a
   "**Try it yourself**" task.
5. **Check** — a self-test question or a checklist to confirm understanding.

### 6.5 Include active-learning prompts

Passive reading produces shallow learning. Sprinkle in active prompts:

```markdown
- **Try it yourself**: Open a terminal and run `docker --version`. What do you see?
```

```markdown
- **Check your understanding**: In your own words, what is the difference between
  **vertical** and **horizontal** scaling?
```

- Use **"Try it yourself"** for hands-on steps and **"Check your understanding"** for
  recall/reflection questions. Both are normal bold-lead-in list items, so they also work
  cleanly in presentation mode (see section 9).
- Where relevant, pose a **before-you-read question** at the start of a chapter
  (*"What happens to a container's data when the container is deleted?"*) and answer it
  by the chapter's end.

### 6.6 Recap with summaries (spaced retrieval)

- Every chapter ends in a `> Summary` blockquote (section 3.8). This is not decoration — it
  is **retrieval practice**. Make the summary re-state the chapter's key ideas in the
  student's own terms, and end with a short bulleted recap of the takeaways.
- Reuse the chapter's bold key terms in the summary so the terminology sticks.

---

## 7. Multimodal learning — vary the mode of engagement

Students learn differently, and retention improves when the same idea is encoded through
**multiple channels**. StudyOER natively supports text, images, video, code, math, lists,
and tables — use a deliberate mix within each chapter, not a single dominant medium.

### 7.1 Textual / verbal (reading–writing)

- Prose and bold-lead-in lists are the backbone. This mode suits **definitions,
  explanations, comparisons, and step sequences**.
- Keep it the default, but never the *only* mode for a chapter.

### 7.2 Visual (diagrams and images)

- Use `./files/…` diagrams for **architecture, flow, structure, and relationships** — the
  kind of knowledge that is spatial and hard to hold in prose.
- Prefer **SVG or sharp PNG** produced specifically for the topic; a generic stock photo
  adds little. Every image should *do* teaching work.
- Anchor every image in the text: introduce it with a sentence, then place the image, then
  (optionally) unpack what to notice in it.

### 7.3 Auditory / dynamic (video)

- Use YouTube embeds (section 3.5) to **demonstrate processes**, narrate a walkthrough, or
  provide a guest/industry perspective — things that are better *shown moving* than read.
- Always give the video a one-line caption explaining what the learner will get from it.
- Never make video the *sole* carrier of essential facts — it should reinforce, not
  replace, the written explanation.

### 7.4 Kinaesthetic / active (hands-on)

- ```` ```code ```` ` blocks are for **running, copying, and modifying** — the most active
  mode. Students type, run, and observe output.
- Pair code with an immediate "**Try it yourself**" prompt so the code is not just read
  but *used*.
- Where possible, give a **predict → observe** exercise: ask the student to predict the
  output before running the command.

### 7.5 Symbolic / formal (math)

- Use ```` ```tex ```` ` blocks (section 3.4) for formulas and formal notation, so math is
  rendered properly instead of as broken text.
- Always explain the *symbols* in words around the formula — a formula alone is
  meaningless to most learners. Name each variable in a following list or sentence.

### 7.6 Choosing the right mode per concept

| To teach… | Best mode(s) |
|---|---|
| A definition or term | text + bold lead-in list |
| A process or workflow | ordered list + diagram |
| A spatial / architectural idea | diagram (SVG/PNG) |
| A live walkthrough or demo | video |
| A runnable command or script | code block + "Try it yourself" |
| A formal rule or formula | `tex` block + prose explanation of symbols |

**Rule of thumb:** every chapter should use **at least two** modes, and any concept that
is hard for learners should intentionally hit **three** modes (e.g. explain in prose,
show as a diagram, then let them run it).

---

## 8. Readability and reading enjoyment

A script that is technically correct but a chore to read fails its purpose. Write so a
tired student at the end of a long day can still follow, stay oriented, and — ideally —
enjoy it.

### 8.1 Plain, concrete language

- Use **short sentences** (mostly under ~25 words) and everyday words over jargon.
- When a technical term is unavoidable, **define it in plain words the first time**, then
  use it consistently afterward. Don't introduce a synonym later.
- Prefer **active voice** and **people doing things** (*"you deploy the app"*) over
  passive abstractions (*"the app is deployed"*).
- Avoid filler ("It is important to note that…", "As we have seen…"). Lead with the point.

### 8.2 Make it scannable

- **Learners skim before they read.** Use headings as signposts so a student can glance
  down the page and grasp the structure in seconds.
- Put the **key term or conclusion at the start** of a paragraph or list item (the bold
  lead-in does this automatically in lists).
- Prefer **lists over walls of prose** for anything enumerable: features, steps, pros/cons,
  comparisons, definitions.
- Use **bold to highlight the words your eye should catch** — but don't bold everything,
  or nothing stands out.

### 8.3 Use analogies and concrete examples

- Map unfamiliar ideas to familiar ones (*"a container image is a recipe; a container is
  a running dish made from it"*). A good analogy pays for itself many times over in
  comprehension.
- Follow any abstraction with a **tangible example**. A named tool or a small scenario is
  more memorable than a definition alone.

### 8.4 Engage the reader as a participant

- Address the reader as **"you"** in explanations and instructions — it invites them into
  the lesson rather than describing it from afar.
- Weave in **questions** (rhetorical or self-check) to break passive reading and provoke
  thought before revealing the answer.
- Vary rhythm: alternate short, punchy sentences with slightly longer ones; alternate prose
  with lists, images, and code so the page never feels monotonous.

### 8.5 Signpost where the learner is

- Give chapters and headings **descriptive, actionable titles** ("**Azure — Creating an
  Account**", not "**Part 2**"). The Table of Contents *is* the learner's map — make every
  entry self-explanatory.
- Open a chapter with a sentence that says what will be learned and why it matters, and
  close with the summary that ties it back to the objectives.

---

## 9. Presentation mode — write for it

StudyOER has a **presentation mode** (projector button) used by lecturers. It changes
how *your* Markdown is displayed. You must author content so it works in both modes.

**How it works in the app:**
- Toggling it flips a `presentation` flag and broadcasts a
  `PresentationModeChangedCommand`; every component (lists, code, images, videos,
  tables, math) subscribes and re-styles itself (typically: bigger, cleaner, fewer
  controls).
- **Navigation:** `ArrowUp` / `ArrowDown` jump between `<section>` elements — i.e.
  **between headings**. `Escape` exits. This means your headings are the "slides."
  Each H1/H2/H3 is a navigation step, so keep sections reasonably self-contained and
  not absurdly long.
- **List "reveal" behavior (the key authoring rule):** For bullet/ordered items written
  as `**Term:** explanation`, the renderer hides the explanation after the colon in
  presentation mode and shows only a large bold bullet, revealing the rest on demand.
  Concretely:
  - The bold lead-in becomes a **48px** "bullet" (• for bullets, or a counter for
    ordered lists).
  - The colon + trailing explanation is wrapped in `.liTextOptional` and **hidden**.
  - Anything you wrap in a *non-bold* explanation after `**Term:**` is the hidden part.
  - **Inside summaries (`> blockquote`) the optional text stays visible** (no hiding).

  **Therefore:** always phrase list items as `**Lead-in term:** optional detail` so
  presentation mode can produce a clean, progressive-reveal slide. Avoid long items with
  no bold lead-in, and avoid critical info that lives only *outside* the lead-in text,
  because it may be hidden during presenting.
- **H1 headings** render as full-width colored banner bands in presentation mode — a
  strong visual chapter separator. Rely on H1 for chapter boundaries, not for small
  headings.
- **Code, images, videos, tables, math** all enlarge/clean up in presentation mode.
  Videos resize to nearly full height; images become larger; code hides its copy button.

**Authoring checklist for presentation mode:**
1. Every chapter is an H1, every slide-worthy topic is an H2/H3.
2. Every list item is `**Term:** detail`, where **Term is the actual content** — not a
   category word like *Strength*, *Weakness*, *Advantage* or *Typical use* (section 3.3).
   Read your bullets aloud: if the slide would just say "Strength, Weakness, Typical
   use", rewrite them.
3. No essential facts exist only in the "detail" tail of a list item.
4. Summaries use `>` blockquotes (their detail is never hidden).
5. **No session numbers, week numbers or dates in headings** (section 3.1) — the pace of a
   real lab changes and the structure must survive it.

---

## 10. How to verify that a script renders

Never hand a script over without checking that StudyOER actually renders it. Markdown
that looks fine in an editor can silently fail in the renderer (truncated headings, bad
table syntax, wrong video URL shape, unsupported construct).

### 10.1 Fastest loop: open a LOCAL file (no commit required)

StudyOER has a built-in **"open local file"** card, so you can preview a `.md` file
straight from disk — **no commit, no push, no public URL needed.** This is the preferred
verification loop while iterating.

**Manual steps (in a browser):**
1. Go to <https://studyoer.logicbystein.com/script>.
2. Click the **"+ " card** (the last card in the grid, icon `mdi-plus`) — it opens a file
   chooser restricted to `.md` / `.markdown`.
3. Pick your local file. The app reads it with `FileReader`, stores it in memory
   (`DataStore.loadedScriptText`) and navigates to `/script/file%3A%2F%2F<encoded path>`.
4. The script renders exactly as a published one does — same renderer, same components,
   same presentation mode.

**Automated steps (Playwright / any browser automation):**
The chooser is a hidden `<input type="file" id="customScriptFileInput">`, so you can drive
it directly without clicking:

```js
await page.goto('https://studyoer.logicbystein.com/script', { waitUntil: 'networkidle' });
await page.setInputFiles('#customScriptFileInput', 'C:\\path\\to\\Script.md');
await page.waitForTimeout(3500); // render + mount components
```

> **Important:** do this **from the `/script` list page**, not from an already-open script
> page — the input only exists on the list page. If you are already on a script page,
> `goto` the list page again first.

**Limits of this mode (know these before trusting a result):**
- Local `./files/…` images and videos **will not load**, because there is no public base
  URL to resolve them against. Text, headings, lists, tables, code, `tex` math and
  YouTube embeds all render normally — but **image paths must be verified separately**
  (commit, or open the raw URL).
- The content lives only in your browser tab's memory. **A `file://` URL is not
  shareable and does not survive a reload** — reopening it in a fresh tab shows an empty
  page whose body is the literal word `undefined`.
- The page title falls back to the file name (`C:\fakepath\Skill.md` in the header), which
  is expected, not an error.

### 10.2 Verifying a published script (public URL)

Once a script should be visible to students, verify it via the render URL:

```
https://studyoer.logicbystein.com/script/<encodeURIComponent(the raw .md URL)>
```

- Source URL for this repository:
  `https://raw.githubusercontent.com/SteiningerSebastian/SteiningerScripts/refs/heads/main/<File%20Name.md>`
- **Watch out for double-encoding.** The filename's own `%20` must be encoded again when
  it becomes part of the route: `Lab. Virtualization and DevOps.md` →
  `Lab.%2520Virtualization%2520and%2520DevOps.md`.
- The file must be **committed and pushed** and reachable on `raw.githubusercontent.com`.
- Publishing is driven by the library manifest
  (`…/StudyOER/refs/heads/main/content.json`), which lists each script's `src` — a script
  can render by URL without being in the manifest, but students only find it via the list.

### 10.3 Verification checklist - what correct looks like

Confirm **all** of these:

1. **No infinite spinner** — a permanently spinning progress indicator means the fetch
   failed (wrong URL, 404, or an unreadable local file).
2. **Body is not `undefined`** — the literal text `undefined` means the content never
   arrived (typically a `file://` URL opened outside its original session).
3. **Table of Contents is populated and complete** — your H1–H3 headings appear,
   auto-numbered (`1.`, `1.1.`, `1.1.1.`), and **each entry shows its full heading text**.
   A shortened entry means the heading contains inline markup (section 10.6).
4. **Every image displays** — no broken image icons (only checkable via a public URL).
5. **Every video shows a player**, not a broken thumbnail.
6. **Every table renders as a styled table** with a header row — not raw pipes.
7. **Code blocks are highlighted** with a copy button; `tex` blocks are typeset math.
   Confirm there is **no** `Undefined control sequence` anywhere in the rendered text —
   that always means the whitespace bug in section 3.4.
8. **Presentation mode works**: click the projector button, then `ArrowDown` / `ArrowUp`
   to walk the headings and `Escape` to exit. List items should collapse to bold bullets
   and nothing essential should vanish. Verify that **every visible bullet still makes
   sense on its own** (section 3.3).

**How to drive presentation mode in automation.** The toolbar has several icon buttons;
pick them by icon class, never by index:

```js
await page.click('button:has(i.mdi-presentation-play)');   // ✅ presentation
// ❌ do not use: button:has(i.mdi-printer-outline)  → print dialog
// ❌ do not use: button:has(i.mdi-format-size)      → text size
```

Then assert the reveal behaviour instead of eyeballing it:

```js
// hidden while presenting
const hidden = await page.$$eval('.liTextOptional',
  els => els.every(e => getComputedStyle(e).display === 'none'));

// every list item still shows something meaningful
const empties = await page.$$eval('li', lis => lis.filter(li => {
  const walk = n => n.nodeType === 3 ? n.textContent
    : (n.classList?.contains('liTextOptional') || getComputedStyle(n).display === 'none')
      ? '' : [...n.childNodes].map(walk).join('');
  return walk(li).trim().length < 3;
}).length);
```

`hidden === true` and `empties === 0` is the pass condition. `Escape` restores
`display: inline` on the same elements.

### 10.4 Failure signatures and their causes

| Symptom | Likely cause |
|---|---|
| Spinner spins forever | Source URL 404 / not public / not yet pushed |
| Body shows `undefined` | Content never fetched, or a `file://` URL opened outside its session |
| **Heading / TOC entry is cut short** | **Inline markup or a quote/apostrophe inside the heading (section 10.6)** |
| Heading missing from TOC | Heading uses `####` or deeper |
| TOC entry contains a session or week number | Structural headings were tied to a schedule (section 3.1) |
| Table shows as raw `\|` text | Missing `\|---\|---\|` separator row, or a pipe inside a cell |
| Table cell shows literal backticks or asterisks | Inline markup inside a table cell (section 3.6) |
| Image is broken | Wrong relative path, or asset not committed under `files/` |
| Image is broken although the asset exists | The linked asset is a **PDF**, not an image (section 3.5) |
| Video shows as a plain image | Thumbnail URL is not `img.youtube.com/vi/<id>/…jpg` |
| Math shows raw `\sqrt{…}` | Code block not tagged `tex` |
| Math shows `Undefined control sequence` | **Whitespace stripped inside a `tex` block** — a command fused with the next letter (section 3.4) |
| A `tex` block renders but other formulas on the page break | One bad block can spoil the batch; check every block for the same bug |
| List item loses text while presenting | No bold lead-in before the colon (section 9) |
| Bullets only say "Strength / Weakness / Typical use" | The lead-in names a category instead of the content (section 3.3) |
| Download link goes nowhere | The archive was never committed under `files/`, or the link points at a folder |
| Download link opens a viewer instead of downloading | Relative `./files/…` path or a `blob/` GitHub URL; use the **`raw/refs/heads/main/`** URL |
| Students cannot follow a referenced program | The script mentions code that is not included or downloadable (section 3.5) |

### 10.5 Quick verification routine

**While drafting (no commit):**
1. Open <https://studyoer.logicbystein.com/script> and use the **"+" card** to load the
   local file (or `setInputFiles('#customScriptFileInput', …)` in automation).
2. Run checklist 10.3 items 1–3, 6–8 (skip images/videos — they need a public URL).

**Before publishing:**
3. Commit and **push** the script and its `files/` assets.
4. Build the raw URL, double-encode it into the render route, open it.
5. Re-run the full checklist 10.3, now including images and videos.

### 10.6 Known renderer bug: headings with inline markup are truncated

**Verified in production (2026-11-01).** `VuetifyHTMLRenderer.heading()` builds the
TOC/label text from `node.firstChild.literal` — i.e. **only the first inline child** of
the heading. Because a heading containing inline markup is split into several inline
children, everything from the marker onward is dropped in the Table of Contents and in
the `#section` anchor id (while the visible `<h1>`–`<h3>` body text still shows fully).

Observed results for otherwise identical headings (all verified live):

| Markdown heading | TOC / anchor shows |
|---|---|
| `## Topic with **bold** word` | `Topic with ` ❌ |
| `## Topic with `code` word` | `Topic with ` ❌ |
| `## Topic with *italic* word` | `Topic with ` ❌ |
| `## Topic with [a link](…) here` | `Topic with ` ❌ |
| `## Topic with "double quotes"` | `Topic with ` ❌ |
| `## Topic with it's apostrophe` | `Topic with it` ❌ |
| `## the learner's own goal` | `the learner` ❌ |
| `## Topic with an em dash — and colon: here` | full text ✅ |
| `## Plain topic (with parenthesis)` | full text ✅ |
| `## dont worry its fine` | full text ✅ |

**Rules that follow from this:**
- **Keep headings to plain text, letters, spaces and safe punctuation.**
- **Banned in headings:** `**`, `` ` ``, `*`, `_`, `[`, `'`, `"`.
- **Safe in headings:** letters, digits, spaces, `-`, `:`, `.`, `,`, `(`, `)`, `—`, `/`, `&`.
- Avoid `>` and `<` in headings too — they can be misread as HTML.
- Move emphasis *out* of the heading and into the first sentence beneath it. If a term must
  be emphasised, do it in a heading-free sentence, e.g. below the heading write
  "**Note:** …".
- Prefer `-` or `:` over possessive apostrophes — write `## The learner goal` or
  `## Goals of the learner` instead of `## The learner's goal`.
- This bug is why a heading like `## 1. What a "script" actually is` appears as
  `1.1. 1. What a` in the TOC — check every TOC entry for truncation.
- The `#section` anchor id is truncated the same way, so a truncated TOC link still jumps
  to the right place, but the link text looks broken to students.

---

## 11. Do and Do Not quick reference

**Do:**
- Start with `# Learning Objectives` → `## Knowledge and Understanding` +
  `## Skills and Abilities` (bold-lead-in bullets).
- Use `#` for chapters, `##`/`###` for topics; rely on auto-numbering.
- Keep headings **topic based and schedule free** — no session, week or date numbers.
- Use `**bold**` generously for key terms.
- Use `**Term:** explanation` for every list item, where **Term is the content**.
- Use `./files/...` for local images; YouTube thumbnails for videos.
- Render a PDF to one PNG per page and discuss each page; never embed the PDF as an image.
- Ship downloadable code as a zip under `files/` and link it with the **absolute GitHub
  `/raw/refs/heads/main/` URL**; never reference code the student cannot obtain.
- Use ```tex``` code blocks for math, with `\,` between a command and a following letter.
- Use ```<language>``` for code (common highlight.js language names).
- End every chapter with a `> Summary` blockquote.
- Separate paragraphs and list items with blank lines.
- Keep headings plain text — no bold, inline code, italics, or links (section 10.6).
- **Verify the render** before handing the script over: load the local file via the "+"
  card on `/script` while drafting, then the public URL before publishing (section 10).
- When a lab uses hardware, add a **generic fallback** note so an exercise still works on
  a plain development board or in a simulator.

**Don't:**
- Don't hand-number headings.
- Don't put session, week or date numbers in headings.
- Don't use `####`+ expecting TOC/presentation visibility.
- Don't use GFM syntax the renderer can't handle: task lists, strikethrough, footnotes,
  inline `` `$...$` `` math, HTML, nested tables.
- Don't put pipes or Markdown formatting inside table cells.
- Don't omit the `|---|---|---|` separator row in tables.
- Don't write list items without a bold lead-in (breaks presentation reveal).
- Don't use a category word (`Strength`, `Weakness`, `Advantage`, `Typical use`) as the
  bold lead-in — the slide will show only the category.
- Don't put critical content only in the hidden "tail" of list items.
- Don't put `**bold**`, `` `code` ``, `*italic*`, or links inside a heading — the TOC entry
  and anchor are silently truncated (section 10.6).
- Don't put a space directly before a letter inside a `tex` block (section 3.4).
- Don't rely on `\text{}` or `\newline` inside `tex`.
- Don't embed a PDF where an image is expected.
- Don't mention a reference implementation you have not shipped.
- Don't ask students to probe a live battery or charger circuit with a multimeter. If a
  measurement can damage hardware or injure a student, describe the safe observation
  instead (a reading printed by the firmware) and drop the physical task.
- Don't link a downloadable zip with a relative `./files/…` path — it renders as a page
  route and does not download. Use the absolute GitHub raw URL instead.
- Don't share a `file://` URL — it only works in the uploader's own tab and renders
  `undefined` anywhere else (section 10.1).

---

## 12. One-shot reference to copy

**Video:** `[![Title](https://img.youtube.com/vi/VIDEO_ID/0.jpg)](https://www.youtube.com/embed/VIDEO_ID)`

**Image:** `![Alt text](./files/name.png)`

**Math:** ```` ```tex\nformula\n``` ````

**List item:** `- **Term:** explanation.`

**Table:**
```markdown
|A|B|C|
|---|---|---|
|1|2|3|
```

**Summary:**
```markdown
> Summary
> Recap text.
```
