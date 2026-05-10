describe('Shift', () => {
  beforeEach(() => {
    cy.deleteAllButArs();
    cy.createDemoEntities();
    cy.createDatabaseInfoForShifts();
  });

  afterEach(() => {
    cy.deleteAllButArs();
  });

  it('create shift', () => {
    const LOCATION = 'Community Center at Lisbon Downtown Area';
    const PARTICIPANTS_LIMIT = '5';

    const now = new Date();
    const tomorrow = new Date(now);
    tomorrow.setDate(now.getDate() + 1);
    const startDate = new Date().getDate();
    const endDate = tomorrow.getDate();

    cy.demoMemberLogin()
    cy.intercept('POST', '/activities/1/shift').as('createShift');
    cy.intercept('GET', '/activities/1/shifts').as('getShifts');

    // Navigate to activities
    cy.get('[data-cy="institution"]').click();
    cy.get('[data-cy="activities"]').click();

    // Navigate to manage shifts for the activity created in beforeEach
    cy.get('[data-cy="memberActivitiesTable"] tbody tr')
      .eq(0)
      .within(() => {
        // Click manage shifts button (calendar icon in actions column)
        cy.get('.action-button').eq(1).click();
      });
    cy.wait('@getShifts');

    // Create shift
    cy.get('[data-cy="newShift"]').click();

    // Fill shift form - start date
    cy.get('#startingDateInput-input').click();
    cy.get('#startingDateInput-wrapper.date-time-picker')
      .contains('.datepicker-day-text', startDate.toString())
      .click({force: true});

    // Fill shift form - end date
    cy.get('#endingDateInput-input').click();
    cy.get('#endingDateInput-wrapper.date-time-picker')
      .contains('.datepicker-day-text', endDate.toString())
      .click({force: true});

    // Fill shift form - location and participants limit
    cy.get('[data-cy="locationInput"]').type(LOCATION);
    cy.get('[data-cy="participantsLimitInput"]').type(PARTICIPANTS_LIMIT);

    // Save shift
    cy.get('[data-cy="saveShift"]').click();
    cy.wait('@createShift');

    // Verify shift was created in the table
    cy.get('[data-cy="activityShiftsTable"] tbody tr:not(.v-data-table__empty-wrapper)')
      .should('have.length', 1)
      .eq(0)
      .children()
      .should('have.length', 4)

    // Verify participants limit
    cy.get('[data-cy="activityShiftsTable"] tbody tr')
      .eq(0)
      .children()
      .eq(2)
      .should('contain', PARTICIPANTS_LIMIT);

    // Verify location
    cy.get('[data-cy="activityShiftsTable"] tbody tr')
      .eq(0)
      .children()
      .eq(3)
      .should('contain', LOCATION);

    cy.logout();
  });

  it('create shift with location with less than 20 characters', () => {
    const LOCATION = 'Lisbon';
    const PARTICIPANTS_LIMIT = '5';

    const now = new Date();
    const tomorrow = new Date(now);
    tomorrow.setDate(now.getDate() + 1);
    const startDate = new Date().getDate();
    const endDate = tomorrow.getDate();

    cy.demoMemberLogin()
    cy.intercept('GET', '/activities/1/shifts').as('getShifts');

    // Navigate to activities
    cy.get('[data-cy="institution"]').click();
    cy.get('[data-cy="activities"]').click();

    // Navigate to manage shifts for the activity created in beforeEach
    cy.get('[data-cy="memberActivitiesTable"] tbody tr')
      .eq(0)
      .within(() => {
        // Click manage shifts button (calendar icon in actions column)
        cy.get('.action-button').eq(1).click();
      });
    cy.wait('@getShifts');

    // Create shift
    cy.get('[data-cy="newShift"]').click();

    // Fill shift form - start date
    cy.get('#startingDateInput-input').click();
    cy.get('#startingDateInput-wrapper.date-time-picker')
      .contains('.datepicker-day-text', startDate.toString())
      .click({force: true});

    // Fill shift form - end date
    cy.get('#endingDateInput-input').click();
    cy.get('#endingDateInput-wrapper.date-time-picker')
      .contains('.datepicker-day-text', endDate.toString())
      .click({force: true});

    // Fill shift form - location (invalid) 
    cy.get('[data-cy="locationInput"]').type(LOCATION);

    // Fill shift form - participants limit
    cy.get('[data-cy="participantsLimitInput"]').type(PARTICIPANTS_LIMIT);

    // Save shift - should be disabled 
    cy.get('[data-cy="saveShift"]').should('be.disabled');

    cy.logout();
  });

it('create shift with location with more than 200 characters', () => {
    const LOCATION = 'Lisbon Lisbon Lisbon Lisbon Lisbon Lisbon Lisbon Lisbon Lisbon Lisbon\
          Lisbon Lisbon Lisbon Lisbon Lisbon Lisbon Lisbon Lisbon Lisbon Lisbon Lisbon Lisbon\
          Lisbon Lisbon Lisbon Lisbon Lisbon Lisbon Lisbon Lisbon Lisbon Lisbon Lisbon Lisbon ';
    const PARTICIPANTS_LIMIT = '5';

    const now = new Date();
    const tomorrow = new Date(now);
    tomorrow.setDate(now.getDate() + 1);
    const startDate = new Date().getDate();
    const endDate = tomorrow.getDate();

    cy.demoMemberLogin()
    cy.intercept('GET', '/activities/1/shifts').as('getShifts');

    // Navigate to activities
    cy.get('[data-cy="institution"]').click();
    cy.get('[data-cy="activities"]').click();

    // Navigate to manage shifts for the activity created in beforeEach
    cy.get('[data-cy="memberActivitiesTable"] tbody tr')
      .eq(0)
      .within(() => {
        // Click manage shifts button (calendar icon in actions column)
        cy.get('.action-button').eq(1).click();
      });
    cy.wait('@getShifts');

    // Create shift
    cy.get('[data-cy="newShift"]').click();

    // Fill shift form - start date
    cy.get('#startingDateInput-input').click();
    cy.get('#startingDateInput-wrapper.date-time-picker')
      .contains('.datepicker-day-text', startDate.toString())
      .click({force: true});

    // Fill shift form - end date
    cy.get('#endingDateInput-input').click();
    cy.get('#endingDateInput-wrapper.date-time-picker')
      .contains('.datepicker-day-text', endDate.toString())
      .click({force: true});

    // Fill shift form - location (invalid) 
    cy.get('[data-cy="locationInput"]').type(LOCATION);

    // Fill shift form - participants limit
    cy.get('[data-cy="participantsLimitInput"]').type(PARTICIPANTS_LIMIT);

    // Save shift - should be disabled 
    cy.get('[data-cy="saveShift"]').should('be.disabled');

    cy.logout();
  });


  it('create shift with start time after end time', () => {
    const LOCATION = 'Community Center at Lisbon Downtown Area';
    const PARTICIPANTS_LIMIT = '5';

    const now = new Date();
    const today = new Date(now);
    today.setDate(now.getDate());
    const yesterday = new Date(now);
    yesterday.setDate(now.getDate() - 1);
    const startDate = today.getDate();
    const endDate = yesterday.getDate();

    cy.demoMemberLogin()
    cy.intercept('POST', '/activities/1/shift', { statusCode: 400 }).as('createShiftError');
    cy.intercept('GET', '/activities/1/shifts').as('getShifts');

    // Navigate to activities
    cy.get('[data-cy="institution"]').click();
    cy.get('[data-cy="activities"]').click();

    // Navigate to manage shifts for the activity created in beforeEach
    cy.get('[data-cy="memberActivitiesTable"] tbody tr')
      .eq(0)
      .within(() => {
        // Click manage shifts button (calendar icon in actions column)
        cy.get('.action-button').eq(1).click();
      });
    cy.wait('@getShifts');

    // Create shift
    cy.get('[data-cy="newShift"]').click();

    // Fill shift form - start date
    cy.get('#startingDateInput-input').click();
    cy.get('#startingDateInput-wrapper.date-time-picker')
      .contains('.datepicker-day-text', startDate.toString())
      .click({force: true});

    // Fill shift form - end date (before start date - invalid)
    cy.get('#endingDateInput-input').click();
    cy.get('#endingDateInput-wrapper.date-time-picker')
      .contains('.datepicker-day-text', endDate.toString())
      .click({force: true});

    // Fill shift form - location and participants limit
    cy.get('[data-cy="locationInput"]').type(LOCATION);
    cy.get('[data-cy="participantsLimitInput"]').type(PARTICIPANTS_LIMIT);

    // Save shift - should fail with 400 error
    cy.get('[data-cy="saveShift"]').click();
    cy.wait('@createShiftError');

    cy.logout();
  });

  it('create shift with dates outside activity date range', () => {
    const LOCATION = 'Community Center at Lisbon Downtown Area';
    const PARTICIPANTS_LIMIT = '5';

    const now = new Date();
    const pastDate = new Date(now);
    pastDate.setDate(now.getDate() - 3);
    const futureDate = new Date(now);
    futureDate.setDate(now.getDate() + 5);
    const startDate = pastDate.getDate();
    const endDate = futureDate.getDate();

    cy.demoMemberLogin()
    cy.intercept('POST', '/activities/1/shift', { statusCode: 400 }).as('createShiftError');
    cy.intercept('GET', '/activities/1/shifts').as('getShifts');

    // Navigate to activities
    cy.get('[data-cy="institution"]').click();
    cy.get('[data-cy="activities"]').click();

    // Navigate to manage shifts for the activity created in beforeEach
    cy.get('[data-cy="memberActivitiesTable"] tbody tr')
      .eq(0)
      .within(() => {
        // Click manage shifts button (calendar icon in actions column)
        cy.get('.action-button').eq(1).click();
      });
    cy.wait('@getShifts');

    // Create shift
    cy.get('[data-cy="newShift"]').click();

    // Fill shift form - start date (before activity start)
    cy.get('#startingDateInput-input').click();
    cy.get('#startingDateInput-wrapper.date-time-picker')
      .contains('.datepicker-day-text', startDate.toString())
      .click({force: true});

    // Fill shift form - end date (after activity end)
    cy.get('#endingDateInput-input').click();
    cy.get('#endingDateInput-wrapper.date-time-picker')
      .contains('.datepicker-day-text', endDate.toString())
      .click({force: true});

    // Fill shift form - location and participants limit
    cy.get('[data-cy="locationInput"]').type(LOCATION);
    cy.get('[data-cy="participantsLimitInput"]').type(PARTICIPANTS_LIMIT);

    // Save shift - should fail with 400 error (dates outside activity range)
    cy.get('[data-cy="saveShift"]').click();
    cy.wait('@createShiftError');

    cy.logout();
  });

  it('new shift button is disabled when activity is suspended', () => {
    const SUSPENSION_JUSTIFICATION = 'Activity suspended for testing';

    cy.demoMemberLogin()
    cy.intercept('GET', '/activities/1/shifts').as('getShifts');

    // Navigate to activities
    cy.get('[data-cy="institution"]').click();
    cy.get('[data-cy="activities"]').click();

    // Navigate to manage shifts for the activity created in beforeEach
    cy.get('[data-cy="memberActivitiesTable"] tbody tr')
      .eq(0)
      .within(() => {
        cy.get('.action-button').eq(1).click();
      });
    cy.wait('@getShifts');

    // Verify "New Shift" button is enabled
    cy.get('[data-cy="newShift"]').should('not.be.disabled');

    // Go back to activities
    cy.get('[data-cy="getActivities"]').click();

    // Suspend the activity
    cy.get('[data-cy="memberActivitiesTable"] tbody tr')
      .eq(0)
      .within(() => {
        cy.get('[data-cy="suspendButton"]').click();
      });

    cy.get('[data-cy="suspensionReasonInput"]').type(SUSPENSION_JUSTIFICATION);
    cy.get('[data-cy="suspendActivity"]').click();

    // Navigate back to manage shifts for the suspended activity
    cy.get('[data-cy="memberActivitiesTable"] tbody tr')
      .eq(0)
      .within(() => {
        cy.get('.action-button').eq(1).click();
      });
    cy.wait('@getShifts');

    // Verify "New Shift" button is now disabled
    cy.get('[data-cy="newShift"]').should('be.disabled');

    cy.logout();
  });

  
  it('create two shifts with total participants greater than activity limit', () => {

    // Create first shift
    const LOCATION = 'Community Center at Lisbon Downtown Area';
    const PARTICIPANTS_LIMIT = '3';

    const now = new Date();
    const tomorrow = new Date(now);
    tomorrow.setDate(now.getDate() + 1);
    const startDate = new Date().getDate();
    const endDate = tomorrow.getDate();

    cy.demoMemberLogin()
    cy.intercept('POST', '/activities/1/shift').as('createShift');
    cy.intercept('GET', '/activities/1/shifts').as('getShifts');

    // Navigate to activities
    cy.get('[data-cy="institution"]').click();
    cy.get('[data-cy="activities"]').click();

    // Navigate to manage shifts for the activity created in beforeEach
    cy.get('[data-cy="memberActivitiesTable"] tbody tr')
      .eq(0)
      .within(() => {
        // Click manage shifts button (calendar icon in actions column)
        cy.get('.action-button').eq(1).click();
      });
    cy.wait('@getShifts');

    // Create shift
    cy.get('[data-cy="newShift"]').click();

    // Fill shift form - start date
    cy.get('#startingDateInput-input').click();
    cy.get('#startingDateInput-wrapper.date-time-picker')
      .contains('.datepicker-day-text', startDate.toString())
      .click({force: true});

    // Fill shift form - end date
    cy.get('#endingDateInput-input').click();
    cy.get('#endingDateInput-wrapper.date-time-picker')
      .contains('.datepicker-day-text', endDate.toString())
      .click({force: true});

    // Fill shift form - location and participants limit
    cy.get('[data-cy="locationInput"]').type(LOCATION);
    cy.get('[data-cy="participantsLimitInput"]').type(PARTICIPANTS_LIMIT);

    // Save shift
    cy.get('[data-cy="saveShift"]').click();
    cy.wait('@createShift');

    // Verify shift was created in the table
    cy.get('[data-cy="activityShiftsTable"] tbody tr:not(.v-data-table__empty-wrapper)')
      .should('have.length', 1)
      .eq(0)
      .children()
      .should('have.length', 4)

    // Verify participants limit
    cy.get('[data-cy="activityShiftsTable"] tbody tr')
      .eq(0)
      .children()
      .eq(2)
      .should('contain', PARTICIPANTS_LIMIT);

    // Verify location
    cy.get('[data-cy="activityShiftsTable"] tbody tr')
      .eq(0)
      .children()
      .eq(3)
      .should('contain', LOCATION);


    // Create second shift
    const SECOND_LOCATION = 'Community Center at Lisbon Downtown Area';
    const SECOND_PARTICIPANTS_LIMIT = '3';

    const second_now = new Date();
    const second_tomorrow = new Date(now);
    second_tomorrow.setDate(now.getDate() + 1);
    const second_startDate = new Date().getDate();
    const second_endDate = tomorrow.getDate();

    cy.intercept('POST', '/activities/1/shift', { statusCode: 400 }).as('createShiftError');
    cy.intercept('GET', '/activities/1/shifts').as('getShifts');

    // Create shift
    cy.get('[data-cy="newShift"]').click();

    // Fill shift form - start date
    cy.get('#startingDateInput-input').click();
    cy.get('#startingDateInput-wrapper.date-time-picker')
      .contains('.datepicker-day-text', startDate.toString())
      .click({force: true});

    // Fill shift form - end date
    cy.get('#endingDateInput-input').click();
    cy.get('#endingDateInput-wrapper.date-time-picker')
      .contains('.datepicker-day-text', endDate.toString())
      .click({force: true});

    // Fill shift form - location 
    cy.get('[data-cy="locationInput"]').type(LOCATION);

    // Fill shift form - participants limit - invalid
    cy.get('[data-cy="participantsLimitInput"]').type(PARTICIPANTS_LIMIT);

    // Save shift - should fail with 400 error
    cy.get('[data-cy="saveShift"]').click();
    cy.wait('@createShiftError');

    cy.logout();
  });



});


