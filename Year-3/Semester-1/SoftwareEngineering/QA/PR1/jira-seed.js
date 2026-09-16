// Seeds a Jira Cloud project from backlog.json using the logged-in browser session.
// Run inside the Jira site origin: node build-seed.js produces seed.run.js for Playwright.
async (page) => {
  const DATA = __DATA__;
  const ARTEM_EMAIL = __ARTEM_EMAIL__;

  return await page.evaluate(async ({ DATA, ARTEM_EMAIL }) => {
    const log = [];
    const api = async (method, path, body) => {
      const res = await fetch(path, {
        method,
        headers: { 'Content-Type': 'application/json', 'Accept': 'application/json', 'X-Atlassian-Token': 'no-check' },
        body: body ? JSON.stringify(body) : undefined,
      });
      const text = await res.text();
      if (!res.ok) throw new Error(`${method} ${path} -> ${res.status}: ${text.slice(0, 400)}`);
      return text ? JSON.parse(text) : null;
    };
    const adf = (paragraphs, bullets) => ({
      type: 'doc', version: 1,
      content: [
        ...paragraphs.map(t => ({ type: 'paragraph', content: [{ type: 'text', text: t }] })),
        ...(bullets && bullets.length ? [{
          type: 'bulletList',
          content: bullets.map(b => ({ type: 'listItem', content: [{ type: 'paragraph', content: [{ type: 'text', text: b }] }] })),
        }] : []),
      ],
    });

    const me = await api('GET', '/rest/api/3/myself');
    const people = { nazar: me.accountId };

    if (ARTEM_EMAIL) {
      const found = await api('GET', `/rest/api/3/user/search?query=${encodeURIComponent(ARTEM_EMAIL)}`);
      if (found.length) people.artem = found[0].accountId;
      else {
        const created = await api('POST', '/rest/api/3/user', { emailAddress: ARTEM_EMAIL, products: ['jira-software'] });
        people.artem = created.accountId;
      }
      log.push(`artem account: ${people.artem}`);
    }

    // Project (team-managed Scrum, so board columns can be added straight from the board UI)
    let project;
    try {
      project = await api('GET', `/rest/api/3/project/${DATA.project.key}`);
      log.push(`project exists: ${project.key}`);
    } catch {
      project = await api('POST', '/rest/api/3/project', {
        key: DATA.project.key,
        name: DATA.project.name,
        description: DATA.project.title,
        projectTypeKey: 'software',
        projectTemplateKey: 'com.pyxis.greenhopper.jira:gh-simplified-agility-scrum',
        leadAccountId: me.accountId,
        assigneeType: 'UNASSIGNED',
      });
      log.push(`project created: ${project.key}`);
    }

    if (people.artem) {
      const roles = await api('GET', `/rest/api/3/project/${DATA.project.key}/role`);
      const memberRole = Object.entries(roles).find(([n]) => /member/i.test(n));
      if (memberRole) {
        const roleId = memberRole[1].split('/').pop();
        try { await api('POST', `/rest/api/3/project/${DATA.project.key}/role/${roleId}`, { user: [people.artem] }); }
        catch (e) { log.push(`role add: ${e.message}`); }
      }
    }

    const types = await api('GET', `/rest/api/3/issue/createmeta/${DATA.project.key}/issuetypes`);
    const typeId = (re) => (types.issueTypes || types.values).find(t => re.test(t.name)).id;
    const T = { epic: typeId(/^(epic|епік|эпик)/i), story: typeId(/^(story|історія|история)/i), task: typeId(/^(task|завдання|задача)$/i), sub: typeId(/sub-?task|підзавдання|подзадача/i) };

    const fields = await api('GET', '/rest/api/3/field');
    const fieldId = (re) => (fields.find(f => re.test(f.name)) || {}).id;
    const SP = fieldId(/^story point estimate$/i) || fieldId(/^story points$/i);
    const START = fieldId(/^start date$/i);

    const create = async (f) => (await api('POST', '/rest/api/3/issue', { fields: { project: { key: DATA.project.key }, ...f } })).key;

    const epicKeys = {};
    for (const e of DATA.epics) {
      const f = { issuetype: { id: T.epic }, summary: e.summary, description: adf([e.description]), duedate: e.due };
      if (START) f[START] = e.start;
      epicKeys[e.key] = await create(f);
    }
    log.push(`epics: ${Object.values(epicKeys).join(', ')}`);

    const storyKeys = [];
    for (const s of DATA.stories) {
      const f = {
        issuetype: { id: T.story }, summary: s.summary, parent: { key: epicKeys[s.epic] },
        priority: { name: s.priority },
        description: adf(['Критерії приймання:'], s.ac),
      };
      if (people[s.assignee]) f.assignee = { accountId: people[s.assignee] };
      let key;
      try { key = await create({ ...f, ...(SP ? { [SP]: s.sp } : {}) }); }
      catch { delete f.priority; key = await create({ ...f, ...(SP ? { [SP]: s.sp } : {}) }); }
      storyKeys.push(key);
    }
    log.push(`stories: ${storyKeys[0]}..${storyKeys.at(-1)}`);

    for (const t of DATA.tasks) {
      const f = { issuetype: { id: T.sub }, summary: t.summary, parent: { key: storyKeys[t.story] } };
      if (people[t.assignee]) f.assignee = { accountId: people[t.assignee] };
      await create(f);
    }
    log.push(`subtasks: ${DATA.tasks.length}`);

    const boards = await api('GET', `/rest/agile/1.0/board?projectKeyOrId=${DATA.project.key}`);
    const boardId = boards.values[0].id;
    // the signup wizard leaves one active + one future sprint; reuse the active one as sprint 1
    const existing = (await api('GET', `/rest/agile/1.0/board/${boardId}/sprint`)).values
      .sort((a, b) => (a.state === 'active' ? -1 : b.state === 'active' ? 1 : a.id - b.id));
    const sprintIds = [];
    for (const [i, sp] of DATA.sprints.entries()) {
      let s = existing[i];
      const body = { name: sp.name, goal: sp.goal, startDate: `${sp.start}T09:00:00.000+03:00`, endDate: `${sp.end}T21:00:00.000+03:00` };
      if (s) s = await api('POST', `/rest/agile/1.0/sprint/${s.id}`, body);
      else s = await api('POST', '/rest/agile/1.0/sprint', { ...body, originBoardId: boardId });
      sprintIds.push(s.id);
    }
    for (const [i, s] of DATA.stories.entries()) {
      await api('POST', `/rest/agile/1.0/sprint/${sprintIds[s.sprint - 1]}/issue`, { issues: [storyKeys[i]] });
    }
    const s1 = DATA.sprints[0];
    if (existing[0]?.state !== 'active') await api('POST', `/rest/agile/1.0/sprint/${sprintIds[0]}`, {
      state: 'active', name: s1.name, goal: s1.goal,
      startDate: `${s1.start}T09:00:00.000+03:00`, endDate: `${s1.end}T21:00:00.000+03:00`,
    });
    log.push(`board ${boardId}, sprints ${sprintIds.join(',')}, sprint 1 active`);
    return { log, boardId, epicKeys, storyKeys };
  }, { DATA, ARTEM_EMAIL });
}
