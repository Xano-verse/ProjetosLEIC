<template>
  <v-card class="table">
    <div class="text-h3">{{ activity.name }}</div>
    <v-data-table
      :headers="headers"
      :items="shifts"
      :search="search"
      disable-pagination
      :hide-default-footer="true"
      :mobile-breakpoint="0"
      data-cy="activityShiftsTable"
    >
      <template v-slot:top>
        <v-card-title>
          <v-text-field
            v-model="search"
            append-icon="search"
            label="Search"
            class="mx-2"
          />
          <v-spacer />
          <!-- Return to Activities button -->
          <v-btn
            color="primary"
            dark
            @click="getActivities"
            data-cy="getActivities"
            >Activities</v-btn
          >
          <!-- New Shift button -->
          <v-btn
            color="primary"
            @click="newShift"
            :disabled="isNewShiftDisabled"
            data-cy="newShift"
          >
            New Shift
          </v-btn>
        </v-card-title>
      </template>

      <!-- Format Dates to a more readable format -->
      <template v-slot:[`item.startTime`]="{ item }">
          {{ showTime((item.startTime)) }}
      </template>

      <template v-slot:[`item.endTime`]="{ item }">
          {{ showTime((item.endTime)) }}
      </template>


    </v-data-table>

   <shift-dialog
      v-if="currentShift"
      :shift="currentShift"
      v-model="currentShift"
      v-on:save-shift="onSaveShift"
      v-on:close-shift-dialog="onCloseShiftDialog"
    />
  </v-card>
</template>

<script lang="ts">
import { Component, Vue } from 'vue-property-decorator';
import { ISOtoString } from '@/services/ConvertDateService';
import RemoteServices from '@/services/RemoteServices';
import Activity from '@/models/activity/Activity';
import Shift from '@/models/shift/Shift';
import ShiftDialog from '@/views/member/ShiftDialog.vue';

@Component({
  components: {
    'shift-dialog': ShiftDialog,
  },
})
export default class InstitutionActivityShiftsView extends Vue {
  activity!: Activity;
  shifts: Shift[] = [];
  search: string = '';

  currentShift: Shift | null = null;

  headers: object = [
    {
      text: 'Start Date',
      value: 'startTime',
      align: 'left',
      width: '25%',
    },
    {
      text: 'End Date',
      value: 'endTime',
      align: 'left',
      width: '25%',
    },
    {
      text: 'Participants Limit',
      value: 'participantsLimit',
      align: 'left',
      width: '20%',
    },
    {
      text: 'Location',
      value: 'location',
      align: 'left',
      width: '20%',
    },

  ];

  get isNewShiftDisabled(): boolean {
    return this.activity?.state !== 'APPROVED';
  }

  async created() {
    this.activity = this.$store.getters.getActivity;

    if (this.activity !== null && this.activity.id !== null) {
      await this.$store.dispatch('loading');
      try {
        //const activityId = this.$route.params.activityId;
        this.shifts = await RemoteServices.getActivityShifts(Number(this.activity.id));
      } catch (error) {
        await this.$store.dispatch('error', error);
      }
      await this.$store.dispatch('clearLoading');
    }
  }

  newShift() {
    this.currentShift = new Shift();
  }

  onCloseShiftDialog() {
    this.currentShift = null;
  }

  async onSaveShift(shift: Shift) {
    await this.$store.dispatch('loading');
    try {
      await RemoteServices.createShift(Number(this.activity.id), shift);
      // Refresh table data
      const activityId = this.$route.params.activityId;
      this.shifts = await RemoteServices.getActivityShifts(Number(activityId));
      

    } catch (error) {
      await this.$store.dispatch('error', error);
    }
    await this.$store.dispatch('clearLoading');
    this.currentShift = null;
  }

  async getActivities() {
    await this.$store.dispatch('setActivity', null);
    this.$router.push({ name: 'institution-activities' }).catch(() => {});
  }

  showTime(time: string) {
    return ISOtoString(time);
  }

}
</script>

<style lang="scss" scoped>
.date-fields-container {
  display: flex;
  flex-direction: column;
  align-items: flex-start;
}

.date-fields-row {
  display: flex;
  gap: 16px;
  margin-top: 8px;
}
</style>
