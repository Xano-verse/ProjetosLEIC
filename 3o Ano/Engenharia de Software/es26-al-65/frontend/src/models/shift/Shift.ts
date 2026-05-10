export default class Shift {
  id: number | null = null;
  startTime!: string;
  endTime!: string;
  participantsLimit!: number;
  location!: string;
  activityId!: string;

  constructor(jsonObj?: Shift) {
    if (jsonObj) {
      this.id = jsonObj.id ?? null
      this.startTime = jsonObj.startTime
      this.endTime = jsonObj.endTime
      this.participantsLimit = jsonObj.participantsLimit
      this.location = jsonObj.location
      this.activityId = jsonObj.activityId
    }
  }
}