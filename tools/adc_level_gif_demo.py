from pathlib import Path
import math

from PIL import Image, ImageDraw, ImageFont


OUTPUT_PATH = Path(r"D:\0000fzrj\xm1视频问题\adc_level_smooth_demo.gif")

WIDTH = 960
HEIGHT = 540
MAX_ADC = 4096
STEP_COUNT = 33
OLD_DISTANCE = 5
NEW_DISTANCE = 15
FILTER_SHIFT = 2
STABLE_COUNT = 5
MAX_STEP_DELTA = 1

BG = (246, 248, 251)
INK = (31, 41, 55)
MUTED = (100, 116, 139)
PANEL = (255, 255, 255)
PANEL_BORDER = (209, 213, 219)
OLD = (220, 38, 38)
OLD_SOFT = (254, 226, 226)
NEW = (22, 163, 74)
NEW_SOFT = (220, 252, 231)
TRACK = (226, 232, 240)
KNOB = (30, 41, 59)
KNOB_MARK = (59, 130, 246)


def load_font(size, bold=False):
    candidates = [
        r"C:\Windows\Fonts\msyhbd.ttc" if bold else r"C:\Windows\Fonts\msyh.ttc",
        r"C:\Windows\Fonts\arialbd.ttf" if bold else r"C:\Windows\Fonts\arial.ttf",
    ]
    for candidate in candidates:
        try:
            return ImageFont.truetype(candidate, size)
        except OSError:
            pass
    return ImageFont.load_default()


FONT_TITLE = load_font(30, True)
FONT_HEAD = load_font(22, True)
FONT_BODY = load_font(17)
FONT_SMALL = load_font(14)
FONT_NUM = load_font(24, True)


def adc_to_step(value, distance):
    for step in range(STEP_COUNT):
        low = MAX_ADC // STEP_COUNT * step - distance
        high = MAX_ADC // STEP_COUNT * (step + 1) + distance
        if step == 0:
            low = 0
        if step == STEP_COUNT - 1:
            high = MAX_ADC
        if low <= value <= high:
            return step
    return None


def simulate_before():
    return [adc_to_step(0, OLD_DISTANCE), adc_to_step(2048, OLD_DISTANCE)]


def simulate_after():
    raw_values = [0] * 8 + [2048] * 90
    outputs = []
    store = 0xFF
    target = 0xFF
    repeat = 0
    filtered = 0
    filter_init = False

    for raw in raw_values:
        if not filter_init:
            filtered = raw
            filter_init = True
        else:
            filtered = ((filtered * ((1 << FILTER_SHIFT) - 1)) + raw) >> FILTER_SHIFT

        step = adc_to_step(filtered, NEW_DISTANCE)
        if step is None:
            continue

        if step != target:
            repeat += 1
            if repeat > STABLE_COUNT:
                target = step
                repeat = 0
        else:
            repeat = 0

        message = None
        if store == 0xFF:
            if target != 0xFF:
                store = target
                message = store
        elif target != 0xFF and target != store:
            current = store
            if target > current:
                current += MAX_STEP_DELTA
                if current > target:
                    current = target
            else:
                current = current - MAX_STEP_DELTA if current > MAX_STEP_DELTA else 0
                if current < target:
                    current = target
            if current != store:
                store = current
                message = store

        if message is not None and (not outputs or outputs[-1] != message):
            outputs.append(message)
        if outputs and outputs[-1] == 16:
            break

    return outputs


def verify_simulation():
    before = simulate_before()
    after = simulate_after()
    assert before == [0, 16], before
    assert after[0] == 0 and after[-1] == 16, after
    assert all(abs(b - a) <= 1 for a, b in zip(after, after[1:])), after


def step_for_frame(sequence, frame_index, hold_first=8, hold_each=4):
    if frame_index < hold_first:
        return sequence[0]
    index = min((frame_index - hold_first) // hold_each, len(sequence) - 1)
    return sequence[index]


def draw_round_rect(draw, box, radius, fill, outline=None, width=1):
    draw.rounded_rectangle(box, radius=radius, fill=fill, outline=outline, width=width)


def draw_centered(draw, xy, text, font, fill):
    bbox = draw.textbbox((0, 0), text, font=font)
    x = xy[0] - (bbox[2] - bbox[0]) / 2
    y = xy[1] - (bbox[3] - bbox[1]) / 2
    draw.text((x, y), text, font=font, fill=fill)


def draw_knob(draw, center, radius, step, color):
    cx, cy = center
    draw.ellipse((cx - radius, cy - radius, cx + radius, cy + radius), fill=(241, 245, 249), outline=PANEL_BORDER, width=3)
    draw.ellipse((cx - radius + 14, cy - radius + 14, cx + radius - 14, cy + radius - 14), fill=KNOB)
    angle = math.radians(-135 + (270 * step / (STEP_COUNT - 1)))
    marker_len = radius - 23
    mx = cx + math.cos(angle) * marker_len
    my = cy + math.sin(angle) * marker_len
    draw.line((cx, cy, mx, my), fill=color, width=7)
    draw.ellipse((cx - 5, cy - 5, cx + 5, cy + 5), fill=(255, 255, 255))


def draw_level_bar(draw, box, step, color, soft):
    x1, y1, x2, y2 = box
    draw_round_rect(draw, box, 8, TRACK)
    fill_w = int((x2 - x1) * step / (STEP_COUNT - 1))
    if fill_w > 0:
        draw_round_rect(draw, (x1, y1, x1 + fill_w, y2), 8, soft)
        draw_round_rect(draw, (x1, y1, x1 + fill_w, y2), 8, color)
    for i in range(0, STEP_COUNT, 4):
        tx = x1 + int((x2 - x1) * i / (STEP_COUNT - 1))
        draw.line((tx, y2 + 4, tx, y2 + 10), fill=PANEL_BORDER, width=1)
    draw.text((x1, y2 + 16), "0", font=FONT_SMALL, fill=MUTED)
    draw.text((x2 - 18, y2 + 16), "32", font=FONT_SMALL, fill=MUTED)


def draw_panel(draw, box, title, badge, step, target, raw, color, soft, mode_text):
    x1, y1, x2, y2 = box
    draw_round_rect(draw, box, 12, PANEL, PANEL_BORDER, 2)
    draw.text((x1 + 24, y1 + 22), title, font=FONT_HEAD, fill=INK)
    badge_box = (x2 - 136, y1 + 22, x2 - 24, y1 + 52)
    draw_round_rect(draw, badge_box, 15, soft)
    draw_centered(draw, ((badge_box[0] + badge_box[2]) / 2, (badge_box[1] + badge_box[3]) / 2 - 1), badge, FONT_SMALL, color)

    draw_knob(draw, (x1 + 110, y1 + 145), 70, step, color)
    draw.text((x1 + 205, y1 + 86), "Output step", font=FONT_BODY, fill=MUTED)
    draw.text((x1 + 205, y1 + 112), f"{step:02d}", font=FONT_NUM, fill=color)
    draw.text((x1 + 275, y1 + 118), f"/ 32", font=FONT_BODY, fill=MUTED)
    draw.text((x1 + 205, y1 + 154), f"Target: {target:02d}", font=FONT_BODY, fill=INK)
    draw.text((x1 + 205, y1 + 181), f"ADC raw: {raw}", font=FONT_BODY, fill=INK)
    draw.text((x1 + 24, y1 + 237), mode_text, font=FONT_BODY, fill=MUTED)
    draw_level_bar(draw, (x1 + 24, y1 + 272, x2 - 24, y1 + 310), step, color, soft)


def draw_info_strip(draw):
    box = (40, 450, WIDTH - 40, 512)
    draw_round_rect(draw, box, 12, (15, 23, 42))
    items = [
        "ADC raw: 0 -> 2048",
        "filter: 1/4 new + 3/4 history",
        "stable count: 5",
        "max step delta: 1",
    ]
    x = 66
    for item in items:
        draw.text((x, 472), item, font=FONT_SMALL, fill=(226, 232, 240))
        x += 218 if item.startswith("filter") else 188


def make_frame(frame_index, before_sequence, after_sequence):
    image = Image.new("RGB", (WIDTH, HEIGHT), BG)
    draw = ImageDraw.Draw(image)
    draw.text((40, 24), "ADC Level Adjustment: Before vs After", font=FONT_TITLE, fill=INK)
    draw.text((42, 63), "Customer view: no jump. Engineering view: same 0 -> 2048 input, different output path.", font=FONT_BODY, fill=MUTED)

    before_step = before_sequence[0] if frame_index < 18 else before_sequence[1]
    after_step = step_for_frame(after_sequence, frame_index, hold_first=8, hold_each=4)
    after_target = 16 if frame_index >= 8 else 0
    raw = 0 if frame_index < 8 else 2048

    draw_panel(
        draw,
        (40, 105, 458, 420),
        "Before: jump",
        "old logic",
        before_step,
        16 if frame_index >= 18 else 0,
        raw,
        OLD,
        OLD_SOFT,
        "Direct mapping sends target step immediately.",
    )
    draw_panel(
        draw,
        (502, 105, 920, 420),
        "After: smooth",
        "new logic",
        after_step,
        after_target,
        raw,
        NEW,
        NEW_SOFT,
        "Filtered target; output moves 1 step per scan.",
    )

    draw_info_strip(draw)
    return image


def render_gif():
    before = simulate_before()
    after = simulate_after()
    frame_count = 78
    frames = [make_frame(index, before, after) for index in range(frame_count)]
    OUTPUT_PATH.parent.mkdir(parents=True, exist_ok=True)
    frames[0].save(
        OUTPUT_PATH,
        save_all=True,
        append_images=frames[1:],
        duration=70,
        loop=0,
        optimize=False,
    )
    return OUTPUT_PATH


def main():
    verify_simulation()
    output = render_gif()
    print(output)


if __name__ == "__main__":
    main()
